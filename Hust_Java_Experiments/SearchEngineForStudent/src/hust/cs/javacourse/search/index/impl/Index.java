package hust.cs.javacourse.search.index.impl;

import hust.cs.javacourse.search.index.*;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * AbstractIndex的具体实现类
 */
public class Index extends AbstractIndex {
    /**
     * 返回索引的字符串表示
     *
     * @return 索引的字符串表示
     */
    @Override
    public String toString() {
        StringBuilder str = new StringBuilder();
        //显示字典
        str.append("dictionary: \n");
        str.append(this.getDictionary().toString());
        str.append("\n");
        //显示文档id和文档路径的映射关系
        str.append("docId---->docPath Mapping: " + "\n");
        for (Map.Entry<Integer, String> entry : docIdToDocPathMapping.entrySet()) {
            str.append(entry.getKey()).append("  ---->  ").append(entry.getValue()).append("\n");
        }
        //显示倒排索引结构
        str.append("Term---->PostingList: ");
        for (Map.Entry<AbstractTerm, AbstractPostingList> entry : termToPostingListMapping.entrySet()) {
            str.append("\n").append(entry.getKey().toString()).append("  -->  ");
            str.append(entry.getValue().toString());
        }
        return str.toString();
    }

    /**
     * 添加文档到索引，更新索引内部的HashMap
     *
     * @param document ：文档的AbstractDocument子类型表示
     */
    @Override
    public void addDocument(AbstractDocument document) {
        this.docIdToDocPathMapping.put(document.getDocId(),document.getDocPath());

        AbstractTermTuple termTuple;
        AbstractTerm term;
        AbstractPostingList postingList;
        AbstractPosting posting;
        for(int i = 0; i < document.getTupleSize(); i++){
            termTuple = document.getTuple(i);
            term = termTuple.term;
            //如果当前term已被加入Map
            if(this.termToPostingListMapping.containsKey(term)){
                postingList = this.termToPostingListMapping.get(term);
                int pos = postingList.indexOf(document.getDocId());

                //当前单词是第一次加入PostingList
                if(pos == -1){
                    List<Integer> positions = new ArrayList<>();
                    positions.add(termTuple.curPos);
                    posting = new Posting(document.getDocId(),1,positions);
                    postingList.add(posting);
                }
                else{
                    posting = postingList.get(pos);
                    posting.setFreq(posting.getFreq()+1);
                    posting.getPositions().add(termTuple.curPos);
                }
            }
            //当前term未被加入map
            else{
                List<Integer> positions = new ArrayList<>();
                positions.add(termTuple.curPos);
                posting = new Posting(document.getDocId(),1,positions);
                postingList = new PostingList();
                postingList.add(posting);
                this.termToPostingListMapping.put(term,postingList);
            }
        }
    }

    /**
     * <pre>
     * 从索引文件里加载已经构建好的索引.内部调用FileSerializable接口方法readObject即可
     * @param file ：索引文件
     * </pre>
     */
    @Override
    public void load(File file) {
        try {
            FileInputStream inStream = new FileInputStream(file);
            ObjectInputStream objectInputStream = new ObjectInputStream(inStream);
            this.readObject(objectInputStream);
            inStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * <pre>
     * 将在内存里构建好的索引写入到文件. 内部调用FileSerializable接口方法writeObject即可
     * @param file ：写入的目标索引文件
     * </pre>
     */
    @Override
    public void save(File file) {
        try {
            FileOutputStream outStream = new FileOutputStream(file);
            ObjectOutputStream objectOutputStream = new ObjectOutputStream(outStream);
            this.writeObject(objectOutputStream);
            outStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 返回指定单词的PostingList
     *
     * @param term : 指定的单词
     * @return ：指定单词的PostingList;如果索引字典没有该单词，则返回null
     */
    @Override
    public AbstractPostingList search(AbstractTerm term) {
        return this.termToPostingListMapping.get(term);
    }

    /**
     * 返回索引的字典.字典为索引里所有单词的并集
     *
     * @return ：索引中Term列表
     */
    @Override
    public Set<AbstractTerm> getDictionary() {
        return this.termToPostingListMapping.keySet();
    }

    /**
     * <pre>
     * 对索引进行优化，包括：
     *      对索引里每个单词的PostingList按docId从小到大排序
     *      同时对每个Posting里的positions从小到大排序
     * 在内存中把索引构建完后执行该方法
     * </pre>
     */
    @Override
    public void optimize() {
        for (Map.Entry<AbstractTerm, AbstractPostingList> entry : termToPostingListMapping.entrySet()){
            AbstractPostingList postingList = entry.getValue();
            postingList.sort();
            for( int i = 0; i < postingList.size(); i++){
                postingList.get(i).sort();
            }
        }
    }

    /**
     * 根据docId获得对应文档的完全路径名
     *
     * @param docId ：文档id
     * @return : 对应文档的完全路径名
     */
    @Override
    public String getDocName(int docId) {
        return this.docIdToDocPathMapping.get(docId);
    }

    /**
     * 写到二进制文件
     *
     * @param out :输出流对象
     */
    @Override
    public void writeObject(ObjectOutputStream out) {
        try {
            out.writeObject(this);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 从二进制文件读
     *
     * @param in ：输入流对象
     */
    @Override
    public void readObject(ObjectInputStream in) {
        try {
            Index index = ((Index)in.readObject());
            this.termToPostingListMapping = index.termToPostingListMapping;
            this.docIdToDocPathMapping = index.docIdToDocPathMapping;
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
}
