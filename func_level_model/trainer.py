import os
import json
import copy
from pyexpat import features
from sys import stderr
#from importlib_metadata import requires
import random
from unittest import result

import numpy as np
import torch
from sklearn.metrics import f1_score, precision_score, recall_score, accuracy_score
from tqdm import tqdm

from utils import debug
from torch.autograd import Variable

import matplotlib.pyplot as plt

def dump_ggnn(model, num_batches, data_iter, path, save_file):
    with torch.no_grad():
        print("Now dump: ", save_file)
        text = open(os.path.join('/home/mVulPreter/func_level_model/data_loader', save_file), 'w', encoding='utf-8')
        for i in range(num_batches):
            try:
                print(i, '/', num_batches)
                input, _ = data_iter()
                for pdg in input:
                    text.write(os.path.join(path, pdg.name) + '\n')
                    #print('pdg.name:' + pdg.name)
                    for j, sub_pdg in enumerate(pdg.sub_pdg):
                        #print(i)
                        if sub_pdg.name == None:
                            sub_pdg.name = str(random.randint(0,100))
                        #print('sub_pdg.name:' + sub_pdg.name)
                        file_path = os.path.join(path, pdg.name , 'GGNN_' + sub_pdg.name)
                        output, feature = model.cal_ggnn(sub_pdg.graph, cuda=True)
                        output = output.detach().cpu().tolist()
                        feature = feature.detach().cpu().tolist()
                        json_dict = {
                            'output':output,
                            'feature':feature
                        }
                        if not os.path.exists(os.path.join(path, pdg.name)):
                            os.makedirs(os.path.join(path, pdg.name))
                        with open(file_path, 'w', encoding='utf-8') as fp:
                            json.dump(json_dict, fp)
            except:
                print('error')
                continue
        text.close()
    pass

def read_testdata(dataset):
    with open('pdgname.txt','w') as fp:
        for pdg in dataset.test_examples:
            fp.write(pdg.name)
            for i, sub_pdg in enumerate(pdg.sub_pdg):
                if i == pdg.graph_num:
                    break
                fp.write(sub_pdg.name)
            fp.write('@@')


def evaluate(model, loss_function, num_batches, data_iter):
    fp = open('/home/attn_mvul/gnn1_prediction_res/rate040.json','a+')
    model.eval()
    with torch.no_grad():
        _loss = []
        all_predictions, all_targets = [], []
        all_weight = []
        result = dict()
        cnt = 0
        cnt = len(result)
        for i in range(num_batches):
            graph, targets = data_iter()
            for _func in graph:
                func_name = _func.name
                result[func_name] = []
            targets =torch.LongTensor(targets).cuda()
            predictions, weight= model(graph, cuda=True)
            batch_loss = loss_function(predictions, targets)
            _loss.append(batch_loss.detach().cpu().item())
            predictions = predictions.detach().cpu()
            if predictions.ndim == 2:
                all_predictions.extend(np.argmax(predictions.numpy(), axis=-1).tolist())
            else:
                all_predictions.extend(
                    predictions.ge(torch.ones(size=predictions.size()).fill_(0.5)).to(
                        dtype=torch.int32).numpy().tolist()
                )
            all_targets.extend(targets.detach().cpu().numpy().tolist())
            all_weight.extend(weight.detach().cpu().numpy().tolist())
            pre=predictions.ge(torch.ones(size=predictions.size()).fill_(0.5)).to(dtype=torch.int32).numpy().tolist()
            tar=targets.detach().cpu().numpy().tolist()
            n = 0
            for res in list(result.keys())[cnt:]:
                result[res].append(int(tar[n]))
                result[res].append(pre[n])
                n+=1
            cnt += 8
            print(1)
        json.dump(result,fp)
        json.dump(all_targets,fp)
        json.dump(all_weight,fp)
        
    fp.close()
    return accuracy_score(all_targets, all_predictions) * 100, \
            precision_score(all_targets, all_predictions) * 100, \
            recall_score(all_targets, all_predictions) * 100, \
            f1_score(all_targets, all_predictions) * 100
    pass

def evaluate_loss(model, loss_function, num_batches, data_iter, cuda=False):
    model.eval()
    with torch.no_grad():
        _loss = []
        all_predictions, all_targets = [], []
        for i in range(num_batches):
            graph, targets = data_iter()
            targets =torch.LongTensor(targets).cuda()
            predictions = model(graph, cuda=True)
            batch_loss = loss_function(predictions, targets)
            _loss.append(batch_loss.detach().cpu().item())
            predictions = predictions.detach().cpu()
            if predictions.ndim == 2:
                all_predictions.extend(np.argmax(predictions.numpy(), axis=-1).tolist())
            else:
                all_predictions.extend(
                    predictions.ge(torch.ones(size=predictions.size()).fill_(0.5)).to(
                        dtype=torch.int32).numpy().tolist()
                )
            all_targets.extend(targets.long().detach().cpu().numpy().tolist())
        debug('target: %s\n' % all_targets)
        debug('predict: %s\n' % all_predictions)
        model.train()
        #print(all_predictions)
        #print(all_targets)
        return np.mean(_loss).item(), f1_score(all_targets, all_predictions) * 100, accuracy_score(all_targets, all_predictions) * 100
    pass


def evaluate_metrics(model, loss_function, num_batches, data_iter):
    model.eval()
    with torch.no_grad():
        _loss = []
        all_predictions, all_targets = [], []
        for i in range(num_batches):
            graph, targets = data_iter()
            targets =torch.LongTensor(targets).cuda()
            predictions = model(graph, cuda=True)
            batch_loss = loss_function(predictions, targets)
            _loss.append(batch_loss.detach().cpu().item())
            predictions = predictions.detach().cpu()
            if predictions.ndim == 2:
                all_predictions.extend(np.argmax(predictions.numpy(), axis=-1).tolist())
            else:
                all_predictions.extend(
                    predictions.ge(torch.ones(size=predictions.size()).fill_(0.5)).to(
                        dtype=torch.int32).numpy().tolist()
                )
            all_targets.extend(targets.detach().cpu().numpy().tolist())
        debug('target: %s\n' % all_targets)
        debug('predict: %s\n' % all_predictions)
        model.train()
        return accuracy_score(all_targets, all_predictions) * 100, \
               precision_score(all_targets, all_predictions) * 100, \
               recall_score(all_targets, all_predictions) * 100, \
               f1_score(all_targets, all_predictions) * 100
    pass


def train(model, dataset, max_steps, dev_every, loss_function, optimizer, save_path, log_every=50, max_patience=5):
    debug('Start Training')
    train_losses = []
    best_model = None
    patience_counter = 0
    best_f1 = 0
    best_acc = 0
    f11 = []
    accc = []
    step = []
    t_loss = []
    v_loss = []
    try:
        for step_count in range(max_steps):
            model.train()
            model.zero_grad()
            graph, targets = dataset.get_next_train_batch()
            targets = torch.LongTensor(targets).cuda()
            #print(targets)
            #graph = Variable(graph, requires_grad = True) ## ++
            #targets = Variable(targets, requires_grad = True) ## ++ 
            #with torch.no_grad(): ### ++
            predictions = model(graph, cuda=True)
            batch_loss = loss_function(predictions, targets)
            if log_every is not None and (step_count % log_every == log_every - 1):
                debug('Step %d\t\tTrain Loss %10.3f' % (step_count, batch_loss.detach().cpu().item()))
            train_losses.append(batch_loss.detach().cpu().item())
            #batch_loss.requires_grad_()
            batch_loss.backward()
            optimizer.step()
            if step_count % dev_every == (dev_every - 1):
                valid_loss, valid_f1 ,valid_acc = evaluate_loss(model, loss_function, dataset.initialize_test_batch(),
                                                     dataset.get_next_test_batch)
                step.append(step_count)
                t_loss.append(batch_loss.detach().cpu().item())
                v_loss.append(valid_loss)
                f11.append(valid_f1)
                accc.append(valid_acc)
                # 只要 acc 和 f1 均大于50  就保存model
                if valid_acc > 50.0 and valid_f1 > 50.0:
                    ckptname = '007-' + str(dataset.batch_size) +'-' + str(valid_acc) + '-' + str(valid_f1) +'-model.ckpt'
                    _save_ckpt_file = open(save_path + ckptname, 'wb')
                    torch.save(model.state_dict(), _save_ckpt_file)
                    _save_ckpt_file.close()
                # 只要valid_acc比以前高 valid_f1>50 就更新
                if valid_f1 > 50.0 and valid_acc > best_acc:
                    patience_counter = 0
                    best_f1 = valid_f1
                    best_acc = valid_acc
                    best_model = copy.deepcopy(model.state_dict())
                    #_save_file = open(save_path + '-model.bin', 'wb')
                    #torch.save(model.state_dict(), _save_file)
                    #_save_file.close()
                else:
                    patience_counter += 1
                debug('Step %d\t\tTrain Loss %10.3f\tValid Loss%10.3f\tf1: %5.2f\tacc: %5.2f\tPatience %d' % (
                    step_count, np.mean(train_losses).item(), valid_loss, valid_f1, valid_acc, patience_counter))
                debug('=' * 100)
                train_losses = []
                if patience_counter == max_patience:
                    break
    except KeyboardInterrupt:
        debug('Training Interrupted by user!')

    if best_model is not None:
        model.load_state_dict(best_model)
    #_save_file = open(save_path + '-model.bin', 'wb')
    #torch.save(model.state_dict(), _save_file)#_save_file.close()
    _save_ckpt_file = open(save_path + '-model.ckpt', 'wb')
    torch.save(model.state_dict(), _save_ckpt_file)
    _save_ckpt_file.close()
    acc, pr, rc, f1 = evaluate_metrics(model, loss_function, dataset.initialize_test_batch(),
                                       dataset.get_next_test_batch)
    debug('%s\tTest Accuracy: %0.2f\tPrecision: %0.2f\tRecall: %0.2f\tF1: %0.2f' % (save_path, acc, pr, rc, f1))
    debug('=' * 100)
    
    plt.subplot(2, 2, 1)
    plt.plot(step, t_loss)
    plt.title('train_loss')
    
    plt.subplot(2, 2, 2)
    plt.plot(step, v_loss)
    plt.title('valid_loss')

    plt.subplot(2, 2, 3)
    plt.plot(step, f11)
    plt.title('f1')

    plt.subplot(2, 2, 4)
    plt.plot(step, accc)
    plt.title('acc')

    plt.suptitle('Evaluate metrics')
    plt.savefig('evaluate_metrics.jpg')