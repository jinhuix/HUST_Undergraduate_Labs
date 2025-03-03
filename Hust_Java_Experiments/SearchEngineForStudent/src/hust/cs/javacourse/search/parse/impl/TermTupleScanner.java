package hust.cs.javacourse.search.parse.impl;

import hust.cs.javacourse.search.index.AbstractTermTuple;
import hust.cs.javacourse.search.index.impl.Term;
import hust.cs.javacourse.search.index.impl.TermTuple;
import hust.cs.javacourse.search.parse.AbstractTermTupleScanner;
import hust.cs.javacourse.search.util.Config;
import hust.cs.javacourse.search.util.StringSplitter;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * TermTupleScanner是抽象类AbstractTermTupleScanner的子类.
 * 读取所输入文档的每一行，并返回解析该行内容后得到的Term三元组
 */
public class TermTupleScanner extends AbstractTermTupleScanner {

    private List<String> words = new ArrayList<>();
    private int pos = 0;

    /**
     * 缺省构造函数
     */
    public TermTupleScanner() {
        super();
    }

    /**
     * 构造函数
     *
     * @param input ：指定输入流对象，应该关联到一个文本文件
     */
    public TermTupleScanner(BufferedReader input) {
        super(input);
    }

    /**
     * 获得下一个三元组
     *
     * @return: 下一个三元组；如果到了流的末尾，返回null
     */
    @Override
    public AbstractTermTuple next() {
        AbstractTermTuple newTermTuple = new TermTuple();
        StringSplitter splitter = new StringSplitter();
        splitter.setSplitRegex(Config.STRING_SPLITTER_REGEX);
        while (this.words.isEmpty()){
            String str = null;
            try {
                str = input.readLine();
            } catch (IOException e) {
                e.printStackTrace();
            }
            if(str==null)
                return null;
            if(Config.IGNORE_CASE)
                this.words = splitter.splitByRegex(str.toLowerCase());
            else
                this.words = splitter.splitByRegex(str);
        }
        newTermTuple.term = new Term(this.words.get(0));
        this.words.remove(0);
        newTermTuple.curPos = pos;
        pos++;
        return newTermTuple;
    }
}
