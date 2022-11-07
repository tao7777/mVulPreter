import argparse
import os
import json
import joblib
import sys

import numpy as np
import torch
from torch.nn import BCELoss
from torch.optim import Adam

from data_loader.dataset import DataSet
from modules.model import mvulModel, attn_mvul, GGNNSum
from trainer import train, dump_ggnn, evaluate, read_testdata, evaluate_metrics
from utils import tally_param, debug

import torch.nn.functional as F

if __name__ == '__main__':
    seed_num = 1000
    debug("dataset: train: ", str(1000), "test, valid: ",str(100), str(100))
    torch.manual_seed(seed_num)
    np.random.seed(seed_num)
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_type', type=str, help='Type of the model',
                        choices=['attn_mvul', 'ggnn', 'mvulModel'], default='mvulModel')
    parser.add_argument('--dataset', type=str, help='Name of the dataset for experiment.',default='mvulModel')
    # deprecate --- input_dir
    parser.add_argument('--input_dir', type=str, help='Input Directory of the parser',default='/home/mvulpreter/dataset_git')
    parser.add_argument('--node_tag', type=str, help='Name of the node feature.', default='node_features')
    parser.add_argument('--graph_tag', type=str, help='Name of the graph feature.', default='graph')
    parser.add_argument('--label_tag', type=str, help='Name of the label feature.', default='target')
    parser.add_argument('--subpdg_tag', type=str, help='Name of the node feature.', default='subpdg')
    parser.add_argument('--subpdg_num_tag', type=str, help='Name of the node feature.', default='subpdg_num')

    parser.add_argument('--feature_size', type=int, help='Size of feature vector for each node', default=100)
    parser.add_argument('--graph_embed_size', type=int, help='Size of the Graph Embedding', default=200)
    parser.add_argument('--num_steps', type=int, help='Number of steps in GGNN', default=6)
    parser.add_argument('--batch_size', type=int, help='Batch Size for training', default=8)
    parser.add_argument('--task', type=str, help='train or pretrain', default='train')
    #TODO: train file path with GGNN /home/mvulpreter/dataset/GGNN
    parser.add_argument('--data_dir', type=str, help='data_dir is only useful in pretrain', default='/home/mVulPreter/func_level_model/dataset')
    args = parser.parse_args()

    if args.feature_size > args.graph_embed_size:
        print('Warning!!! Graph Embed dimension should be at least equal to the feature dimension.\n'
              'Setting graph embedding size to feature size', file=sys.stderr)
        args.graph_embed_size = args.feature_size

    model_dir = os.path.join('models', args.dataset)
    if not os.path.exists(model_dir):
        os.makedirs(model_dir)
    input_dir = args.input_dir
    processed_data_path = os.path.join('/home/mVulPreter/func_level_model/data_loader', 'dataset_GGNN.pkl')
    if True and os.path.exists(processed_data_path):
        print("#"*20)
        dataset = joblib.load(open(processed_data_path, 'rb'))
        read_testdata(dataset)
        debug('Reading already processed data from %s!' % processed_data_path)
    else:
        with open('/home/mVulPreter/func_level_model/data_loader/split_dataset_GGNN.json', 'r') as fp:
            data = json.load(fp)
        if args.task == 'eval':
            print("$"*20)
            dataset = DataSet(train_src=data['train'],
                                valid_src=data['valid'],
                                test_src=data['test'],
                                batch_size=args.batch_size, n_ident=args.node_tag, g_ident=args.graph_tag,
                                l_ident=args.label_tag, s_ident=args.subpdg_tag, sn_ident=args.subpdg_num_tag,
                                task = args.task)
            file = open(processed_data_path, 'wb')
            joblib.dump(dataset, file)
            file.close()
        else:
            print("$"*20)   
            dataset = DataSet(train_src=data['train'],
                                valid_src=data['valid'],
                                test_src=data['test'],
                                batch_size=args.batch_size, n_ident=args.node_tag, g_ident=args.graph_tag,
                                l_ident=args.label_tag, s_ident=args.subpdg_tag, sn_ident=args.subpdg_num_tag,
                                task = args.task)
            file = open(processed_data_path, 'wb')
            joblib.dump(dataset, file)
            file.close()            
    
    assert args.feature_size == dataset.feature_size, \
        'Dataset contains different feature vector than argument feature size. ' \
        'Either change the feature vector size in argument, or provide different dataset.'

    if args.model_type == 'ggnn':
        model = GGNNSum(input_dim=dataset.feature_size, output_dim=args.graph_embed_size,
                        #num_steps=args.num_steps, max_edge_types=dataset.max_edge_type)
                        num_steps=args.num_steps, max_edge_types=3)
    elif args.model_type == 'attn_mvul':
        model = attn(input_dim=dataset.feature_size, output_dim=args.graph_embed_size,
                            num_steps=args.num_steps, max_edge_types=4)
    elif args.model_type == 'mvulModel':
        model = mvulModel(input_dim=dataset.feature_size, output_dim=args.graph_embed_size,
                            num_steps=args.num_steps, max_edge_types=4)

    debug('Total Parameters : %d' % tally_param(model))
    debug('#' * 100)
    print(model)
    model.cuda()
    loss_function = F.cross_entropy
    # loss_function = BCELoss(reduction='sum')
    optim = Adam(model.parameters(), lr=0.0001, weight_decay=0.001)
    debug("dataset: train: ", str(1000), "test, valid: ",str(100), str(100))
    debug("lr: ", str(0.0001))
    debug("weight_decay: ", str(0.001))
    debug("random seed: ", str(seed_num))
    if args.task == 'train':
        train(model=model, dataset=dataset, max_steps=5300, dev_every=53,
          loss_function=loss_function, optimizer=optim,
          save_path=model_dir + '/GGNNSumModel', max_patience=100, log_every=None)

    elif args.task == 'pretrain':
        model.eval()
        ckpt = torch.load('/home/mVulPreter/slice_level_model/models/008-8-96.03267211201867-74.24242424242425-GGNNmodel_2d.ckpt')
        model.load_state_dict(ckpt)
        dump_ggnn(model, dataset.initialize_train_batch(), dataset.get_next_train_batch, args.data_dir, 'train_GGNN.txt')
        dump_ggnn(model, dataset.initialize_valid_batch(), dataset.get_next_valid_batch, args.data_dir, 'val_GGNN.txt')
        dump_ggnn(model, dataset.initialize_test_batch(), dataset.get_next_test_batch, args.data_dir, 'test_GGNN.txt')

    elif args.task == 'eval':
        model.eval()
        ckpt = torch.load('/home/mVulPreter/func_level_model/models/mvulModel/GGNNSumModel-model.ckpt')
        model.load_state_dict(ckpt)
        acc, pr, rc, f1 = evaluate_metrics(model, loss_function, dataset.initialize_test_batch(), dataset.get_next_test_batch)
        save_path = '/home/mVulPreter/func_level_model/models/mvulModel/GGNNSumModel-model.ckpt\n'
        debug('%s\tTest Accuracy: %0.2f\tPrecision: %0.2f\tRecall: %0.2f\tF1: %0.2f' % (save_path, acc, pr, rc, f1))
        debug('=' * 100)