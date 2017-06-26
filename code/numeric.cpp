//
//  numeric.cpp
//  NetworkEmbedding
//
//  Created by Feng Jun on 16/1/2.
//  Copyright (c) 2016年 Feng Jun. All rights reserved.
//

#include "numeric.h"


map<pair<int, int>, int> testPair;

bool SortFunction(NodeInfo a, NodeInfo b)
{
    return a.count > b.count;
}

void CreateBinaryTree() {
    
    //    sort(nodes.begin(), nodes.end(), SortFunction);
    int cdepth;
    int min1i, min2i, pos1, pos2;
    
    //the main temp data structure to store the points and codes of each leaf node
    //limited by the maximize depth of the tree, might be changed for different data set.
    int point[MAX_CODE_LENGTH];
    int code[MAX_CODE_LENGTH];
    
    //the main data structure used here, storing all the counts of leaf nodes and non-leaf nodes
    int *count = (int *)calloc(nodeNum * 2 + 1, sizeof(int));
    
    
    //the extra data structure, "binary" stores the binary id of each node (leaf & non-leaf), thus, whether it is the left/right children of its parent
    //and "parent_node" stores the nodes' id of each nodes' parent.
    //using the two data structure, we can maintain the tree structure of the Huffman tree
    int *binary = (int *)calloc(nodeNum * 2 + 1, sizeof(int));
    int *parent_node = (int *)calloc(nodeNum * 2 + 1, sizeof(int));
    
    
    //initiate the count array
    //NOTE:
    //after the initiation, the count array looks like:
    //
    // 5 4 3 2 1 100 100 100 100
    //          |_ end of the leaf nodes
    //
    // thus, the leaf nodes are ranked in a decrease order and the non-leaf nodes are all the maximum value, hence there is a valley in the array
    // moreover, the valley will always exist, because all the non-leaf nodes added in should follow a increase order (each new non-leaf node is
    // generated by adding in the two minimum counts.
    // this is why the following Huffman tree generation function is correct
    for (int i = 0; i < nodeNum; i++)		//leaf nodes
    {
        count[i] = nodes[i].count;
    }
    
    for (int i = nodeNum; i < nodeNum * 2; i++) //non-leaf nodes
    {
        count[i] = 1e15;
    }
    
    
    // we start at the end of the leaf nodes, since they must be the minimum two nodes
    // at each iteration, we select the minimum two nodes and kill them by move pos1, pos2
    // to either end of the array. Since the array is a valley and pos1, pos2 are always
    // at the basin, we can guarantee that all minimum node is always among pos1 and pos2
    
    pos1 = nodeNum - 1;
    pos2 = nodeNum;
    
    // Following algorithm constructs the Huffman tree by adding one node at a time
    for (int i = 0; i < nodeNum - 1; i++) {
        
        // First, find two smallest nodes 'min1, min2'
        
        // find the first smallest node and store it, then skip it by moving the pos1 or pos2 index
        if (pos1 >= 0)
        {
            if (count[pos1] < count[pos2])	//find the minimum count
            {
                min1i = pos1;
                pos1--;			//skip pos1
            }
            else
            {
                min1i = pos2;
                pos2++;			//skip pos2
            }
        }
        else
        {
            min1i = pos2;
            pos2++;
        }
        
        //	then we re-do the above process and find the second smallest node.
        if (pos1 >= 0)
        {
            if (count[pos1] < count[pos2])
            {
                min2i = pos1;
                pos1--;
            }
            else
            {
                min2i = pos2;
                pos2++;
            }
        }
        else
        {
            min2i = pos2;
            pos2++;
        }
        
        //we put the new node to the current non-leaf node index with the correct count and parent, binary values
        count[nodeNum + i] = count[min1i] + count[min2i];
        parent_node[min1i] = nodeNum + i;
        parent_node[min2i] = nodeNum + i;
        
        //the smallest node goes to 0 and the second smallest node goes to 1
        binary[min1i] = 0;
        binary[min2i] = 1;
    }
    
    
    // Now assign binary code to each vocabulary word
    for (int i = 0; i < nodeNum; i++)
    {
        int j = i;		//come to the current node that we will operate on.
        
        cdepth = 0;
        
        // first get the depth of each node
        while (1)
        {
            if (cdepth >= 50)
                printf("%d\n", cdepth);
            code[cdepth] = binary[j];
            point[cdepth] = j;
            cdepth ++;
            j = parent_node[j];
            if (j == nodeNum * 2 - 2)	//come to the root of the Huffman tree
            {
                break;
            }
        }
        
        nodes[i].codelen = cdepth;
        nodes[i].point[0] = nodeNum - 2;		//NOTE:  all the nodes' ids on the point array is shifted by vocab_size.
        //       for example, vacab[i].point[0] should be the root of the Huffman tree,
        //       whose id is vocab_size*2 - 2, so we shift it by vocab_size and
        //       result in vocab_size - 2.
        
        
        for (int k = 0; k < cdepth; k ++)
        {
            nodes[i].code[cdepth - k - 1] = code[k];
            nodes[i].point[cdepth - k] = point[k] - nodeNum;		// the ids are shifted by vocab_size, as stated before
        }
        
        //finally, for each leaf node vacab[i], codelen is the depth of the path for root to it.
        //in the code and point array, it stores data as follows,
        //
        //id:      0                1             2           ...    cdepth-2                 cdepth-1
        //point:  root          par of 2       par of 3            par of vocab[i]            vocab[i]
        //code:  edge 0->1      edge 1->2      edge 1->3      edge cdepth-2 -> cdepth -1        n/a
        
    }
    
    
    // free the temporary arrays
    free(count);
    free(binary);
    free(parent_node);
}

void InitExpTable()
{
    int i;
    expTable = (double *)malloc((EXP_TABLE_SIZE + 1) * sizeof(double));
    
    for (i = 0; i < EXP_TABLE_SIZE; i++) {
        expTable[i] = exp((i / (double)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP); // Precompute the exp() table
        expTable[i] = expTable[i] / (expTable[i] + 1);                   // Precompute f(x) = x / (x + 1)
    }
}

int min(int x, int y)
{
    if (x < y)
        return x;
    else return y;
}


double CountTmpScore(int bestOrNot, int type, int centerNode, int cX, int cY, int label)
{
    vector<double> context;
    for (int i = 0; i < dim; i ++)
        context.push_back(0);
    
    double totAttention = 0;
    vector<int> wId;
    vector<double> attention;
    wId.clear();
    
    vector<vector<int> > ps;
    vector<int> targetNode;
    vector<int> p;
    vector<double> pathConf;
    
    p.clear();
    p.push_back(cX), p.push_back(cY);
    ps.push_back(p);
    targetNode.push_back(centerNode);
    pathConf.push_back(1);
    
    for (int i = 0; i < testPathPair[make_pair(cX, centerNode)].size(); i ++)
    {
        int tmp = testPathPair[make_pair(cX, centerNode)][i];
        ps.push_back(paths[tmp].path);
        targetNode.push_back(paths[tmp].centerNode);
        pathConf.push_back(paths[tmp].confidence);
    }
    
    double firstTmp = 0;
    double totTmp = 0;
    for (int id = 0; id < ps.size(); id ++)
    {
        double tmp = 0;
        for (int i = 0; i < dim; i ++)
        {
            context[i] = 0;
        }
        int cw = ps[id].size();
        
        wId.clear();
        // in -> hidden
        for (int i = 0; i < ps[id].size(); i ++)
        {
            
            wId.push_back(ps[id][i]);
        }
        
        if (pathConf[id] != 1)
        {
            totAttention = 0;
            attention.clear();
            for (int i = 0; i < wId.size(); i ++)
            {
                if (attentionLabel == 0)
                    attention.push_back(1);
                if (attentionLabel == 1)
                {
                    attention.push_back(exp(w[wId[i] * maxPathLen + cw - i - 1]));
                }
                
                totAttention += attention[i];
            }
            for (int i = 0; i < wId.size(); i ++)
                attention[i] /= totAttention;
        }

        
        for (int i = 0; i < wId.size(); i ++)
        {
            for (int c = 0; c < dim; c ++)
            {
                if (bestOrNot == 1)
                {
                    if (pathConf[id] != 1)
                        context[c] += attention[i] * best_vector[c + wId[i] * dim];
                    else context[c] += best_vector[c + wId[i] * dim];
                }
                else
                {
                    if (pathConf[id] != 1)
                        context[c] += attention[i] * word_vector[c + wId[i] * dim];
                    else context[c] += word_vector[c + wId[i] * dim];
                }
            }
        }
        for (int c = 0; c < dim; c++)
        {
            context[c] /= cw;
        }
        
        for (int j = 0; j < nodes[targetNode[id]].codelen; j ++)
        {
            double f = 0;
            int l2 = nodes[targetNode[id]].point[j] * dim;
            for (int c = 0; c < dim; c++)
            {
                if (bestOrNot == 1)
                    f += context[c] * best_theta_u[c + l2];
                else f += context[c] * theta_u[c + l2];
            }
            
            //the exp Table only allows input f in (-MAX_EXP, MAX_EXP), otherwise the resultant value
            //index will be invalid.
            //as the result, we should skip the corner cases
            //see the defination of exp Table at the beginning for details
            if (f <= -MAX_EXP)
            {
                f = -MAX_EXP + 1;
                //numSmall ++;
                //continue;
            }
            else if (f >= MAX_EXP)
            {
                f = MAX_EXP - 1;
                //numLarge ++;
                //continue;
            }
            //when the input f is in (-MAX_EXP, MAX_EXP), the output f will be in [0, EXP_TABLE_SIZE], indicating the corresponding value of tao(f)
            f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];
            tmp += (1 - nodes[targetNode[id]].code[j]) * log(f) + nodes[targetNode[id]].code[j] * log(1 - f);
        }
//        if (id == 1)
//            printf("%d ", label);
//        if (ps.size() > 1)
//            printf("%lf ", tmp);
//        totTmp += tmp;
        totTmp += tmp;
        if (id == 0)
            firstTmp = tmp;
    }
    totTmp /= ps.size();
//    if (ps.size() > 1)
//        printf("%lf %lf %d\n", firstTmp, totTmp, label);
    return totTmp;
}

int GetLen(int pathType, int type, int index)
{
    if (pathType == 0)
    {
        if (type == 0 || type == 2)
            return (index + 1) * 2;
        if (type == 1)
        {
            if (index == 0)
                return 2;
            else return index * 2 + 1;
        }
    }
    if (pathType == 1)
    {
        return index + 2;
    }
    return index * 2;
}

int     randomWalk(int st)
{
    int len = (int) headData[st].size();
    // printf("len = %d\n", len);
    double* p = new double[len];
    for (int i = 0; i < len; i ++)
    {
        int tail = headData[st][i].tail;
        if (headData[tail].size() == 0)
        {
            p[i] = p[i - 1];
            continue;
        }
        p[i] = 100000 / headData[tail].size();
        if (i > 0)
            p[i] += p[i - 1];
    }
    double u = ((double) random() / RAND_MAX) * p[len - 1];
    //printf("%lf %lf\n", u, p[len - 1]);
    int res = 0;
    for (; res < len; res ++)
        if (p[res] > u && ((res == 0 && p[res] != 0) || (res != 0 && p[res] != p[res - 1])))
            break;
    return res;
}

void GetEmbedding()
{
    string inputFile = outProcessFile + "embedding_fb15k_random_single_100_5_0.1_120_1_0_1.txt";
    FILE *fin = fopen(inputFile.c_str(), "r");
    for (int i = 0; i < nodeNum; i ++)
    {
        double tmp;
        for (int j = 0; j < dim; j ++)
        {
            fscanf(fin, "%lf", &tmp);
            best_vector.push_back(tmp);
            // printf("%lf ", tmp);
        }
        //  printf("\n");
    }
    for (int i = 0; i < nodeNum; i ++)
    {
        double tmp;
        for (int j = 0; j < dim; j ++)
        {
            fscanf(fin, "%lf", &tmp);
            best_theta_u.push_back(tmp);
            // printf("%lf ", tmp);
        }
        //printf("\n");
    }
    
    double tmp;
    for (int i = 0; i < 4; i ++)
    {
        fscanf(fin, "lf", &tmp);
        bestW.push_back(tmp);
    }
    CreateBinaryTree();
    InitExpTable();
}


