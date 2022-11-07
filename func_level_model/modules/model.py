from pyexpat import features
import torch
from dgl.nn import GatedGraphConv
from torch import nn
import torch.nn.functional as f
import numpy as np
from torch.autograd import Variable

class Attention(nn.Module):
    def __init__(self, dim):
        super(Attention,self).__init__()
        self.linear = nn.Linear(dim*2, dim)
        self.mask = None
        #self.adaptavgpool = torch.nn.AdaptiveAvgPool1d(1)

    def set_mask(self,mask):
        self.mask = torch.ByteTensor(mask).unsqueeze(2).cuda()

    def forward(self, key, query):
        batch_size = key.size(0)
        hidden_size = key.size(2)
        input_size = query.size(1)
        # (batch, key_len, dim) * (batch, query_len, dim) -> (batch, key_len, query_len)
        attn = torch.bmm(key, query.transpose(1, 2))
        if self.mask is not None:
            attn.data.masked_fill(self.mask, -float('inf'))
        attn = f.softmax(attn, dim=1)

        # (batch, key_len, query_len) * (batch, query_len, dim) -> (batch, key_len, dim)
        energy = torch.bmm(attn, query)
        # concat -> (batch, outlen, 2*dim)
        combind = torch.cat((energy,key), dim=2)
        # output -> (batch, out_len, dim)
        output = torch.tanh(self.linear(combind.view(-1, 2 * hidden_size))).view(batch_size, -1, hidden_size)
        return output, attn



class mvulModel(nn.Module):
    def __init__(self, input_dim, output_dim, max_edge_types=2, num_steps=6):
        super(mvulModel, self).__init__()
        self.inp_dim = input_dim #100
        self.out_dim = output_dim #200
        self.max_edge_types = max_edge_types
        self.num_timesteps = num_steps
        self.ggnn = GatedGraphConv(in_feats=input_dim, out_feats=output_dim,
                                   n_steps=num_steps, n_etypes=max_edge_types)
        self.conv_l1 = torch.nn.Conv1d(output_dim, output_dim, 3)
        self.maxpool1 = torch.nn.MaxPool1d(3, stride=2)
        self.conv_l2 = torch.nn.Conv1d(output_dim, output_dim, 1)
        self.maxpool2 = torch.nn.MaxPool1d(2, stride=2)

        self.concat_dim = input_dim + output_dim
        self.conv_l1_for_concat = torch.nn.Conv1d(self.concat_dim, self.concat_dim, 3)
        self.maxpool1_for_concat = torch.nn.MaxPool1d(3, stride=2)
        self.conv_l2_for_concat = torch.nn.Conv1d(self.concat_dim, self.concat_dim, 1)
        self.maxpool2_for_concat = torch.nn.MaxPool1d(2, stride=2)

        self.batchnorm_1d = torch.nn.BatchNorm1d(output_dim)
        self.batchnorm_1d_for_concat = torch.nn.BatchNorm1d(self.concat_dim)

        self.mlp_z = nn.Linear(in_features=self.concat_dim, out_features=2)
        self.mlp_y = nn.Linear(in_features=output_dim, out_features=2)
        self.sigmoid = nn.Sigmoid()

    def get_network_inputs(self, graph, cuda=False, device=None):
        features = graph.ndata['features']
        edge_types = graph.edata['etype']
        if cuda:
            return graph.to(torch.device(device)), features.cuda(device=device), edge_types.cuda(device=device)
        else:
            return graph, features, edge_types

    def cal_ggnn(self, graph, cuda):
        graph, features, edge_types = self.get_network_inputs(graph, cuda=cuda, device="cuda:0")
        output = self.ggnn(graph, features, edge_types)
        return output, features

    def forward(self, batch, cuda=False):
        graph, features, edge_types = batch.get_network_inputs(cuda=cuda)
        graph = graph.to(torch.device("cuda:0"))
        features = features.to(torch.device("cuda:0"))
        edge_types = edge_types.to(torch.device("cuda:0"))
        outputs = self.ggnn(graph, features, edge_types)
        x_i, _ = batch.de_batchify_graphs(features)
        h_i, _ = batch.de_batchify_graphs(outputs)
        c_i = torch.cat((h_i, x_i), dim=-1)
        batch_size, num_node, _ = c_i.size()
        Y_1 = self.maxpool1(
            f.relu(
                self.batchnorm_1d(
                    self.conv_l1(h_i.transpose(1, 2))
                )
            )
        )
        Y_2 = self.maxpool2(
            f.relu(
                self.batchnorm_1d(
                    self.conv_l2(Y_1)
                )
            )
        ).transpose(1, 2)
        Z_1 = self.maxpool1_for_concat(
            f.relu(
                self.batchnorm_1d_for_concat(
                    self.conv_l1_for_concat(c_i.transpose(1, 2))
                )
            )
        )
        Z_2 = self.maxpool2_for_concat(
            f.relu(
                self.batchnorm_1d_for_concat(
                    self.conv_l2_for_concat(Z_1)
                )
            )
        ).transpose(1, 2)
        before_avg = torch.mul(self.mlp_y(Y_2), self.mlp_z(Z_2))
        avg = before_avg.mean(dim=1)
        return self.sigmoid(avg)
        result = self.sigmoid(avg).squeeze(dim=-1)
        return result


class attn_mvul(nn.Module):
    def __init__(self, input_dim, output_dim, max_edge_types=2, num_steps=6):
        super(attn_mvul, self).__init__()
        self.int_dim = input_dim #100
        self.out_dim = output_dim #200
        self.max_edge_types = max_edge_types
        self.num_timesteps = num_steps

        self.attn = Attention(output_dim)
        self.adaptavgpool1 = torch.nn.AdaptiveAvgPool2d((1, input_dim))
        self.adaptavgpool2 = torch.nn.AdaptiveAvgPool2d((1, output_dim))

        self.ggnn = GatedGraphConv(in_feats=input_dim, out_feats=output_dim,
                                   n_steps=num_steps, n_etypes=max_edge_types)
        self.conv_l1 = torch.nn.Conv1d(output_dim, output_dim, 3)
        self.maxpool1 = torch.nn.MaxPool1d(3, stride=2)
        self.conv_l2 = torch.nn.Conv1d(output_dim, output_dim, 1)
        self.maxpool2 = torch.nn.MaxPool1d(2, stride=2)

        self.concat_dim = input_dim + output_dim
        self.conv_l1_for_concat = torch.nn.Conv1d(self.concat_dim, self.concat_dim, 3)
        self.maxpool1_for_concat = torch.nn.MaxPool1d(3, stride=2)
        self.conv_l2_for_concat = torch.nn.Conv1d(self.concat_dim, self.concat_dim, 1)
        self.maxpool2_for_concat = torch.nn.MaxPool1d(2, stride=2)

        self.batchnorm_1d = torch.nn.BatchNorm1d(output_dim)
        self.batchnorm_1d_for_concat = torch.nn.BatchNorm1d(self.concat_dim)


        self.mlp_z = nn.Linear(in_features=self.concat_dim, out_features=2)
        self.mlp_y = nn.Linear(in_features=output_dim, out_features=2)
        self.sigmoid = nn.Sigmoid()

    def de_batchify_graphs(self, features=None):
        if features is None:
            features = self.graph.ndata['features']
        assert isinstance(features, torch.Tensor)

        #vectors = [torch.tensor(1)]
        #vectors[0]= features.clone().datach().requires_grad_(True)
        #vectors[0] = torch.tensor(features,requires_grad = True)
        #output_vectors = torch.stack(vectors)
        return features, None

    # get GGNN's input from graph
    def get_network_inputs(self, graph, cuda=False, device=None):
        features = graph.ndata['features']
        edge_types = graph.edata['etype']
        if cuda:
            return graph.to(torch.device(device)), features.cuda(device=device), edge_types.cuda(device=device)
        else:
            return graph, features, edge_types

    # layer GGNN and avgpool
    def network_GGNN(self, pdg, cuda):
        #graph, features, edge_types = self.get_network_inputs(pdg.graph, cuda=cuda, device="cuda:0")

        outputs = self.adaptavgpool2(pdg.output.unsqueeze(0).cuda()) #
        features = self.adaptavgpool1(pdg.feature.unsqueeze(0).cuda())
        return features.squeeze(0), outputs.squeeze(0)

    # extract 'outputs' and 'features' from input graph
    def get_output_features(self, input, cuda):
        o_outputs = []
        o_features =[]
        for pdg in input:
            outputs = []
            features = []
            for sub_pdg in pdg.sub_pdg:
                feature, output = self.network_GGNN(sub_pdg, cuda=cuda)
                outputs.append(output.squeeze(0))
                features.append(feature.squeeze(0))
            o_outputs.append(torch.stack(outputs))
            o_features.append(torch.stack(features))
        return torch.stack(o_outputs), torch.stack(o_features)

    # set attention mask
    def set_mask(self, input):
        masks = []
        for pdg in input:
            #print(len(pdg.mask))
            #masks.append([[True] * len(pdg.mask) if i else [False] * len(pdg.mask) for i in pdg.mask])
            masks.append(pdg.mask)
        #print(len(masks))
        self.attn.set_mask(masks)

    def forward(self, input, cuda=False):
        outputs, features = self.get_output_features(input=input, cuda=cuda)
        self.set_mask(input)
        #outputs = Variable(outputs, requires_grad=True)
        #features = Variable(features, requires_grad=True)
        outputs, weight = self.attn(outputs, outputs)
        x_i, _ = self.de_batchify_graphs(features)
        h_i, _ = self.de_batchify_graphs(outputs)
        c_i = torch.cat((h_i, x_i), dim=-1)
        batch_size, num_node, _ = c_i.size()
        Y_1 = self.maxpool1(
            f.relu(
                self.batchnorm_1d(
                    self.conv_l1(h_i.transpose(1, 2))
                )
            )
        )
        Y_2 = self.maxpool2(
            f.relu(
                self.batchnorm_1d(
                    self.conv_l2(Y_1)
                )
            )
        ).transpose(1, 2)
        Z_1 = self.maxpool1_for_concat(
            f.relu(
                self.batchnorm_1d_for_concat(
                    self.conv_l1_for_concat(c_i.transpose(1, 2))
                )
            )
        )
        Z_2 = self.maxpool2_for_concat(
            f.relu(
                self.batchnorm_1d_for_concat(
                    self.conv_l2_for_concat(Z_1)
                )
            )
        ).transpose(1, 2)
        before_avg = torch.mul(self.mlp_y(Y_2), self.mlp_z(Z_2))
        avg = before_avg.mean(dim=1)
        return self.sigmoid(avg)
        result = self.sigmoid(avg).squeeze(dim=-1)
        return result, weight


class GGNNSum(nn.Module):
    def __init__(self, input_dim, output_dim, max_edge_types, num_steps=8):
        super(GGNNSum, self).__init__()
        self.inp_dim = input_dim
        self.out_dim = output_dim
        self.max_edge_types = max_edge_types
        self.num_timesteps = num_steps
        self.ggnn = GatedGraphConv(in_feats=input_dim, out_feats=output_dim, n_steps=num_steps,
                                   n_etypes=max_edge_types)
        self.classifier = nn.Linear(in_features=output_dim, out_features=1)
        self.sigmoid = nn.Sigmoid()

    def forward(self, batch, cuda=False):
        graph, features, edge_types = batch.get_network_inputs(cuda=cuda)
        outputs = self.ggnn(graph, features, edge_types)
        h_i, _ = batch.de_batchify_graphs(outputs)
        ggnn_sum = self.classifier(h_i.sum(dim=1))
        result = self.sigmoid(ggnn_sum).squeeze(dim=-1)
        return result