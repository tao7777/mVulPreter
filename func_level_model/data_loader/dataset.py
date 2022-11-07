from cProfile import label
import os
import copy
import json
import sys
from unicodedata import name

import torch
from dgl import DGLGraph
from tqdm import tqdm

from data_loader.batch_graph import GGNNBatchGraph
from utils import load_default_identifiers, initialize_batch, debug

import random

class SubPdgEntry:
    def __init__(self, output, feature, name):
        self.name = name
        self.output = torch.FloatTensor(output)
        self.feature = torch.FloatTensor(feature)


class P_SubPdgEntry:
    def __init__(self, dataset, num_nodes, features, edges, name=None):
        self.dataset = dataset
        self.num_nodes = num_nodes
        self.name = name
        self.graph = DGLGraph()
        self.features = torch.FloatTensor(features)
        self.graph.add_nodes(self.num_nodes, data={"features": self.features})
        for s, _type, t in edges:
            etype_number = self.dataset.get_edge_type_number(_type)
            self.graph.add_edges(s, t, data={"etype": torch.LongTensor([etype_number])})


class PdgEntry:
    def __init__(self, dataset, target, name):
        self.dataset = dataset
        self.graph_num = 0
        self.target = target
        self.sub_pdg = []
        self.mask = []
        self.name = name

    def add_pdg(self, pdg, name, task):
        self.graph_num += 1
        self.mask.append(0)
        if task == 'pretrain':
            self.sub_pdg.append(P_SubPdgEntry(dataset=self.dataset, num_nodes=len(pdg[self.dataset.n_ident]), 
                                            features=pdg[self.dataset.n_ident], edges=pdg[self.dataset.g_ident],
                                            name=name))
        elif task == 'train' or task == 'eval':
            self.sub_pdg.append(SubPdgEntry(output=pdg['output'], feature=pdg['feature'], name=name))

    def padding_pdg(self, max_graph_num):
        if self.graph_num != len(self.mask):
            self.mask = [0]*self.graph_num
            self.sub_pdg = self.sub_pdg[:self.graph_num]
        feature = [[0]*100,[0]*100]
        edge = [[0,0,1],[0,1,1]]
        for i in range(max_graph_num-self.graph_num):
            self.sub_pdg.append(P_SubPdgEntry(dataset=self.dataset,num_nodes=len(feature),
                                            features=feature,edges=edge,name= str(i) + '.json'))
            self.mask.append(1)
        return self

    def set_mask(self):
        self.mask = []
        for sub_pdg in self.sub_pdg:
            if '@' in sub_pdg.name:
                self.mask.append(1)
            else:
                self.mask.append(0)
        return self


class DataSet:
    def __init__(self, train_src, valid_src=None, test_src=None, batch_size=32, input_dim=100,
                 n_ident=None, g_ident=None, l_ident=None, s_ident=None, sn_ident=None, task='train'):
        self.train_examples = []
        self.valid_examples = []
        self.test_examples = []
        self.train_batches = []
        self.valid_batches = []
        self.test_batches = []
        self.batch_size = batch_size
        self.edge_types = {}
        self.max_etype = 0
        self.feature_size = input_dim
        self.n_ident = n_ident
        self.g_ident = g_ident
        self.l_ident = l_ident
        self.s_ident = s_ident
        self.sn_ident = sn_ident
        self.task = task
        self.read_dataset(test_src, train_src, valid_src)
        self.initialize_dataset()

    def initialize_dataset(self):
        self.initialize_train_batch()
        self.initialize_valid_batch()
        self.initialize_test_batch()

    def read_dataset(self, test_src: dict, train_src: dict, valid_src: dict):
        debug('Reading Validation File!')
        if len(valid_src.keys()) != 0:
            for function_name, slice_path_list in tqdm(valid_src.items()):
                if len(slice_path_list) < 8:
                    continue

                example = PdgEntry(dataset=self, target=function_name[0], name=function_name)

                for slice_path in slice_path_list:
                    slice_name = slice_path.split('/')[-1].strip()
                    with open(slice_path, 'r') as fp:
                        slice_json = json.load(fp)
                        if self.task == 'train' or self.task == 'eval':
                            example.add_pdg(slice_json, slice_name, self.task)
                        else:
                            if len(slice_json[self.n_ident]) > 8:
                                example.add_pdg(slice_json, slice_name, self.task)
                        
                if example.graph_num > 8:
                    self.valid_examples.append(example)

        debug('Reading Test File!')
        if len(test_src.keys()) != 0:
            for function_name, slice_path_list in tqdm(test_src.items()):
                if len(slice_path_list) < 8:
                    continue

                example = PdgEntry(dataset=self, target=function_name[0], name=function_name)

                for slice_path in slice_path_list:
                    slice_name = slice_path.split('/')[-1].strip()
                    with open(slice_path, 'r') as fp:
                        slice_json = json.load(fp)
                        if self.task == 'train' or self.task == 'eval':
                            example.add_pdg(slice_json, slice_name, self.task)
                        else:
                            if len(slice_json[self.n_ident]) > 8:
                                example.add_pdg(slice_json, slice_name, self.task)
                        
                if example.graph_num > 8:
                    self.test_examples.append(example)

        debug('Reading Train File!')
        if len(train_src.keys()) != 0:
            for function_name, slice_path_list in tqdm(train_src.items()):
                if len(slice_path_list) < 8:
                    continue

                example = PdgEntry(dataset=self, target=function_name[0], name=function_name)

                for slice_path in slice_path_list:
                    slice_name = slice_path.split('/')[-1].strip()
                    with open(slice_path, 'r') as fp:
                        slice_json = json.load(fp)
                        if self.task == 'train' or self.task == 'eval':
                            example.add_pdg(slice_json, slice_name, self.task)
                        else:
                            if len(slice_json[self.n_ident]) > 8:
                                example.add_pdg(slice_json, slice_name, self.task)
                        
                if example.graph_num > 8:
                    self.train_examples.append(example)

   

    def get_edge_type_number(self, _type):
        if _type not in self.edge_types:
            self.edge_types[_type] = self.max_etype
            self.max_etype += 1
        return self.edge_types[_type]

    @property
    def max_edge_type(self):
        return self.max_etype

    def initialize_train_batch(self, batch_size=-1):
        if batch_size == -1:
            batch_size = self.batch_size
        self.train_batches = initialize_batch(self.train_examples, batch_size, shuffle=False)
        return len(self.train_batches)

    def initialize_valid_batch(self, batch_size=-1):
        if batch_size == -1:
            batch_size = self.batch_size
        self.valid_batches = initialize_batch(self.valid_examples, batch_size)
        return len(self.valid_batches)

    def initialize_test_batch(self, batch_size=-1):
        if batch_size == -1:
            batch_size = self.batch_size
        self.test_batches = initialize_batch(self.test_examples, batch_size)
        return len(self.test_batches)

    def get_dataset_by_ids_for_GGNN(self, entries, ids):
        taken_entries = [entries[i] for i in ids]
        labels = [int(e.target) for e in taken_entries]
        batch_graph = GGNNBatchGraph()
        for entry in taken_entries:
            batch_graph.add_subgraph(copy.deepcopy(entry.graph))
        return batch_graph, torch.FloatTensor(labels)

    def get_dataset_by_ids(self, entries, ids):
        if self.task == 'train' or self.task == 'eval':
            taken_entries = [entries[i].set_mask() for i in ids]
        elif self.task == 'pretrain':
            max_graph_num = 0
            for i in ids:
                max_graph_num = max(max_graph_num,entries[i].graph_num)
            taken_entries = [entries[i].padding_pdg(max_graph_num) for i in ids]
        # labels = [int(e.name[0]) for e in taken_entries]
        labels = [int(e.target) for e in taken_entries]
        return taken_entries, labels


    def get_next_train_batch(self):
        if len(self.train_batches) == 0:
            self.initialize_train_batch()
        ids = self.train_batches.pop()
        entries, labels = self.get_dataset_by_ids(self.train_examples, ids)
        return entries, labels
        
        #if len(self.train_batches) == 0:
        #    self.initialize_train_batch()
        #ids = self.train_batches.pop()
        #return self.get_dataset_by_ids(self.train_examples, ids)

    def get_next_valid_batch(self):
        if len(self.valid_batches) == 0:
            self.initialize_valid_batch()
        ids = self.valid_batches.pop()
        return self.get_dataset_by_ids(self.valid_examples, ids)

    def get_next_test_batch(self):
        if len(self.test_batches) == 0:
            self.initialize_test_batch()
        ids = self.test_batches.pop()
        return self.get_dataset_by_ids(self.test_examples, ids)
