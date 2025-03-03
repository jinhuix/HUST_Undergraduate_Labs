package hust.cs.javacourse.search.index.impl;

import hust.cs.javacourse.search.index.AbstractTermTuple;

/**
 * AbstractTermTuple的具体实现类.
 */
public class TermTuple extends AbstractTermTuple {
    /**
     * 判断二个三元组内容是否相同
     *
     * @param obj ：要比较的另外一个三元组
     * @return 如果内容相等（三个属性内容都相等）返回true，否则返回false
     */
    @Override
    public boolean equals(Object obj) {
        if(obj instanceof TermTuple){
            TermTuple t = (TermTuple)obj;
            return  (this.curPos == t.curPos) &&
                    (this.freq == t.freq) &&
                    (this.term.equals(t.term));
        }
        return false;
    }

    /**
     * 获得三元组的字符串表示
     *
     * @return ： 三元组的字符串表示
     */
    @Override
    public String toString() {
        return "<term:\"" + this.term + "\",freq:" + this.freq + ",curPos:" + this.curPos + ">";
    }
}
