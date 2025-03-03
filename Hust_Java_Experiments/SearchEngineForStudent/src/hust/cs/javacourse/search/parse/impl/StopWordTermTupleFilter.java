package hust.cs.javacourse.search.parse.impl;

import hust.cs.javacourse.search.index.AbstractTermTuple;
import hust.cs.javacourse.search.parse.AbstractTermTupleFilter;
import hust.cs.javacourse.search.parse.AbstractTermTupleStream;
import hust.cs.javacourse.search.util.StopWords;

import java.util.Arrays;

/**
 * StopWordTermTupleFilter是抽象类AbstractTermTupleFilter的子类.
 * 用于过滤含StopWords类中定义的停用词的三元组
 */
public class StopWordTermTupleFilter extends AbstractTermTupleFilter {
    /**
     * 构造函数
     *
     * @param input ：Filter的输入，类型为AbstractTermTupleStream
     */
    public StopWordTermTupleFilter(AbstractTermTupleStream input) {
        super(input);
    }

    /**
     * 获得下一个三元组
     *
     * @return: 下一个三元组；如果到了流的末尾，返回null
     */
    @Override
    public AbstractTermTuple next() {
        AbstractTermTuple temp = input.next();
        while (temp!=null){
            if(!Arrays.asList(StopWords.STOP_WORDS).contains(temp.term.getContent())){
                return temp;
            }
            else{
                temp = input.next();
            }
        }
        return null;
    }
}
