# mVulPreter
We implement mVulPreter, a multi-granularity vulnerability detector that can provide interpretation of detection results. 
Specifically, we propose a novel technique to effectively blend the advantages of function-level and slice-level vulnerability detection models and output the detection results' interpretation only by the model itself. 

# Dataset
The dataset we used is as follows.

```
Jiahao Fan, Yi Li, Shaohua Wang, and Tien Nguyen. 2020. A C/C++ Code Vulnerability Dataset with Code Changes and CVE Summaries. In The 2020 International Conference on Mining Software Repositories (MSR). 
```

You can download it by this link : https://drive.google.com/file/d/1-0VhnHBp9IGh90s2wCNjeCMuy70HPl8X/view?usp=sharing


# Usage
```shell  
mVulPreter 
|-- preprocess        
  |-- code_normalize          // implement the normalization
  |-- raw_data_preprocess.py  // extract the src file from the dataset (.csv) and get the deletions in the patch
  |-- joern_graph_gen.py      // execute joern to generate the representations of the source code
  |-- slice_process           // extract the slices of the function
  |-- train_embedding.py      // train the embedding model 
  |-- joern_to_mvul.py        // convert the source code into the required format of mVulPreter model
|-- slice_level_model 
  |-- data_loader             // load the propocessed data
  |-- main.py                 // the entry of slice-level model
  |-- trainer.py              // train the slice-level model
  |-- utils.py                
  |-- modules                 // the content of the model
|-- func_level_model  
  |-- data_loader             // load the propocessed data
  |-- main.py                 // the entry of function-level model
  |-- trainer.py              // train the function-level model
  |-- utils.py                
  |-- modules                 // the content of the model
|-- utils_dataset     
```
### Environment
- Joern 1.1.172.
You can find it in Joern's historical releases:  https://github.com/joernio/joern
- Networkx 2.4 / 2.5
- Pytorch 1.5

### Step1: Preprocess
- Input:  the raw dataset (csv file)
- Output: the folder of the function before patching, the folder of the function after patching, and the pickle file of deletions of the patch
```
python preprocess/raw_data_preprocess.py
```

- Input:  the folder of the function before patching, the folder of the function after patching
- Output: the normalized code
- Notion: modified the path within the python file
```
python preprocess/code_normalize/normalization.py
```

- Input: the normalized code
- Output: the folder of .bin file
```
python preprocess/joern_graph_gen.py -t pasre
```
- Input: the folder of .bin file
- Output: the folder of pdg.dot of function
```
python preprocess/joern_graph_gen.py -t export -r pdg
```
- Input: the folder of .bin file
- Output: the folder of line_info.json
```
python preprocess/joern_graph_gen.py -t export -r lineinfo_json
```
- Input: line_info.json, pdg.dot, and the pickle file of deletions
- Output: the folder of pdg.dot of slice
```
python prepreprocess/slice_process/main.py
```
- Input: the folder of pdg.dot of slice
- Output: the input of slice-level model
```
python preprocess/train_embedding.py
python preprocess/joern_to_mvul.py
```

### Step2: Slice-level model 
- Input: the input of the slice-level model
- Output: the predicting results of slices (i.e., the filtering result)
- Notion: please split the dataset based on function-level and record the file path into two txt files (train.txt and test.txt). Remember to modify the path in the following python file accordingly.
```
python slice_level_model/main.py
```

### Step3: Function-level model 
- Input: the remained slices of the slice-level model
- Output: the predicting results on the function-level

```
python func_level_model/main.py
```

# Publication
Deqing Zou, Yutao Hu, Wenke Li, Yueming Wu, Haojun Zhao, and Hai Jin. [mVulPreter: A Multi-granularity Vulnerability Detection System with Interpretations.](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=9864301) IEEE Transactions on Dependable and Secure Computing (TDSC). 2022. doi: 10.1109/TDSC.2022.3199769.

If you use our dataset or source code, please kindly cite our paper:

```
@article{mVulPreter,
  author={Zou, Deqing and Hu, Yutao and Li, Wenke and Wu, Yueming and Zhao, Haojun and Jin, Hai},
  journal={IEEE Transactions on Dependable and Secure Computing},
  title={mVulPreter: A Multi-granularity Vulnerability Detection System with Interpretations}, 
  year={2022},
  doi={10.1109/TDSC.2022.3199769}
  }
 
