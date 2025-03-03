package hust.cs.javacourse.search.index.impl;

import hust.cs.javacourse.search.index.AbstractPosting;
import hust.cs.javacourse.search.index.AbstractPostingList;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Collections;
import java.util.List;

/**
 * AbstractPostingList的具体实现类.
 */
public class PostingList extends AbstractPostingList {
    /**
     * 添加Posting,要求不能有内容重复的posting
     *
     * @param posting ：Posting对象
     */
    @Override
    public void add(AbstractPosting posting) {
        if (!this.list.contains(posting)){
            this.list.add(posting);
        }
    }

    /**
     * 获得PosingList的字符串表示
     *
     * @return ： PosingList的字符串表示
     */
    @Override
    public String toString() {
        StringBuilder str = new StringBuilder();
        if (this.list == null)
            return null;
        boolean flag = false;
        for (AbstractPosting p : this.list){
            if (flag){
                str.append("->");
            }
            flag = true;
            str.append(p.toString());
        }
        str.append("\n");
        return str.toString();
    }

    /**
     * 添加Posting列表,,要求不能有内容重复的posting
     *
     * @param postings ：Posting列表
     */
    @Override
    public void add(List<AbstractPosting> postings) {
        AbstractPosting temp;
        for (AbstractPosting p : postings){
            temp = p;
            if (!this.list.contains(temp)){
                this.list.add(temp);
            }
        }
    }

    /**
     * 返回指定下标位置的Posting
     *
     * @param index ：下标
     * @return： 指定下标位置的Posting
     */
    @Override
    public AbstractPosting get(int index) {
        if(index >= 0 && index < this.list.size())
            return this.list.get(index);
        else
            return null;
    }

    /**
     * 返回指定Posting对象的下标
     *
     * @param posting ：指定的Posting对象
     * @return ：如果找到返回对应下标；否则返回-1
     */
    @Override
    public int indexOf(AbstractPosting posting) {
        for (int i=0;i<this.list.size();i++){
            if (this.list.get(i).equals(posting))
                return i;
        }
        return -1;
    }

    /**
     * 返回指定文档id的Posting对象的下标
     *
     * @param docId ：文档id
     * @return ：如果找到返回对应下标；否则返回-1
     */
    @Override
    public int indexOf(int docId) {
        for (int i=0;i<this.list.size();i++){
            if (this.list.get(i).getDocId() == docId)
                return i;
        }
        return -1;
    }

    /**
     * 是否包含指定Posting对象
     *
     * @param posting ： 指定的Posting对象
     * @return : 如果包含返回true，否则返回false
     */
    @Override
    public boolean contains(AbstractPosting posting) {
        return this.list.contains(posting);
    }

    /**
     * 删除指定下标的Posting对象
     *
     * @param index ：指定的下标
     */
    @Override
    public void remove(int index) {
        if(index >= 0 && index < this.list.size())
            this.list.remove(index);
    }

    /**
     * 删除指定的Posting对象
     *
     * @param posting ：定的Posting对象
     */
    @Override
    public void remove(AbstractPosting posting) {
        this.list.remove(posting);
    }

    /**
     * 返回PostingList的大小，即包含的Posting的个数
     *
     * @return ：PostingList的大小
     */
    @Override
    public int size() {
        return this.list.size();
    }

    /**
     * 清除PostingList
     */
    @Override
    public void clear() {
        this.list.clear();
    }

    /**
     * PostingList是否为空
     *
     * @return 为空返回true;否则返回false
     */
    @Override
    public boolean isEmpty() {
        return this.list.isEmpty();
    }

    /**
     * 根据文档id的大小对PostingList进行从小到大的排序
     */
    @Override
    public void sort() {
        Collections.sort(this.list);
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
            this.list = ((PostingList)in.readObject()).list;
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
}
