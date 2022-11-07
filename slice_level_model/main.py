import argparse
import json
import os
import pickle
import sys
import joblib
import numpy as np
import torch
from torch.nn import BCELoss
from torch.optim import Adam

from data_loader.dataset import DataSet
from modules.model import mvulModel, GGNNSum
from trainer import train, eval, filter_slice
from utils import tally_param, debug

from torch_geometric.datasets import Planetoid
import torch_geometric.transforms as T

import torch.nn.functional as F

if __name__ == '__main__':
    torch.manual_seed(22)
    np.random.seed(22)
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_type', type=str, help='Type of the model (mvulModel/ggnn)',
                        choices=['mvulModel', 'ggnn'], default='mvulModel')
    parser.add_argument('--dataset', type=str, help='Name of the dataset for experiment.',default='mvulModel')
    parser.add_argument('--input_dir', type=str, help='Input Directory of the parser',default='/home/mvulpreter/dataset')
    parser.add_argument('--node_tag', type=str, help='Name of the node feature.', default='node_features')
    parser.add_argument('--graph_tag', type=str, help='Name of the graph feature.', default='graph')
    parser.add_argument('--label_tag', type=str, help='Name of the label feature.', default='target')
    parser.add_argument('--subpdg_tag', type=str, help='Name of the node feature.', default='subpdg')
    parser.add_argument('--subpdg_num_tag', type=str, help='Name of the node feature.', default='subpdg_num')

    parser.add_argument('--feature_size', type=int, help='Size of feature vector for each node', default=100)
    parser.add_argument('--graph_embed_size', type=int, help='Size of the Graph Embedding', default=200)
    parser.add_argument('--num_steps', type=int, help='Number of steps in GGNN', default=6)
    parser.add_argument('--batch_size', type=int, help='Batch Size for training', default=8)
    parser.add_argument('--task', type=str, help='train or pretrain', default='eval')

    args = parser.parse_args()
    if args.feature_size > args.graph_embed_size:
        print('Warning!!! Graph Embed dimension should be at least equal to the feature dimension.\n'
              'Setting graph embedding size to feature size', file=sys.stderr)
        args.graph_embed_size = args.feature_size

    model_dir = os.path.join('models', args.dataset)
    if not os.path.exists(model_dir):
        os.makedirs(model_dir)
    input_dir = args.input_dir
    if args.task != 'eval':
        processed_data_path = os.path.join('/home/mVulPreter/slice_level_model/data_loader', 'slice_filter.pkl')
        if True and os.path.exists(processed_data_path):
            print('*'*20)
            dataset = joblib.load(open(processed_data_path, 'rb'))
            #debug(len(dataset.train_examples), len(dataset.valid_examples), len(dataset.test_examples))
            debug('Reading already processed data from %s!' % processed_data_path)
        else:
            print('#'*20)
            with open('/home/mVulPreter/func_level_model/data_loader/split_dataset_pretrain.json', 'r') as fp:
                data = json.load(fp)
            dataset = DataSet(train_src=data['train'],
                            valid_src=data['valid'],
                            test_src=data['test'],
                            batch_size=args.batch_size, n_ident=args.node_tag, g_ident=args.graph_tag,
                            l_ident=args.label_tag)
            file = open(processed_data_path, 'wb')
            joblib.dump(dataset, file)
            file.close()
    else:
        processed_data_path = os.path.join('/home/mVulPreter/slice_level_model/data_loader', 'slice_filter.pkl')
        if True and os.path.exists(processed_data_path):
            print('*'*20)
            dataset = joblib.load(open(processed_data_path, 'rb'))
            #debug(len(dataset.train_examples), len(dataset.valid_examples), len(dataset.test_examples))
            debug('Reading already processed data from %s!' % processed_data_path)
        else:
            print('#'*20)
            with open('/home/mVulPreter/func_level_model/data_loader/split_dataset_pretrain.json', 'r') as fp:
                data = json.load(fp)
            dataset = DataSet(train_src=data['train'],
                            valid_src=data['valid'],
                            test_src=data['test'],
                            batch_size=args.batch_size, n_ident=args.node_tag, g_ident=args.graph_tag,
                            l_ident=args.label_tag)
            file = open(processed_data_path, 'wb')
            joblib.dump(dataset, file)
            file.close()

    assert args.feature_size == dataset.feature_size, \
        'Dataset contains different feature vector than argument feature size. ' \
        'Either change the feature vector size in argument, or provide different dataset.'


    if args.model_type == 'ggnn':
        model = GGNNSum(input_dim=dataset.feature_size, output_dim=args.graph_embed_size,
                        num_steps=args.num_steps, max_edge_types=dataset.max_edge_type)
    else:
        model = mvulModel(input_dim=dataset.feature_size, output_dim=args.graph_embed_size,
                            num_steps=args.num_steps, max_edge_types=4)

    debug('Total Parameters : %d' % tally_param(model))
    debug('#' * 100)
    print(model)
    model.cuda()
    loss_function = F.cross_entropy
    # loss_function = BCELoss(reduction='sum')
    optim = Adam(model.parameters(), lr=0.0001, weight_decay=0.001)
    debug('batch size  : %d' % args.batch_size)
    debug('lr  : 0.0001')
    debug('weight_decay  : 0.0001')
    if args.task == 'eval':
        ckpt = torch.load('/home/mVulPreter/slice_level_model/models/008-8-96.03267211201867-74.24242424242425-GGNNmodel_2d.ckpt')
        model.load_state_dict(ckpt)
        # eval(model=model, dataset=dataset, max_steps=2000, dev_every=15,
        #     loss_function=loss_function, optimizer=optim,
        #     save_path=model_dir , max_patience=100, log_every=None)
        filter_slice(model, loss_function, dataset.initialize_train_batch(), dataset.get_next_train_batch)
        filter_slice(model, loss_function, dataset.initialize_valid_batch(), dataset.get_next_valid_batch)
        filter_slice(model, loss_function, dataset.initialize_test_batch(), dataset.get_next_test_batch)
    else:
        train(model=model, dataset=dataset, max_steps=1000, dev_every=50,
            loss_function=loss_function, optimizer=optim,
            save_path=model_dir , max_patience=50, log_every=None)

