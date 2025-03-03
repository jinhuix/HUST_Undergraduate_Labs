#ifndef STOP_WAIT_RDT_SENDER_H
#define STOP_WAIT_RDT_SENDER_H
#include "RdtSender.h"
#include<deque>

class GBNSender :public RdtSender
{
private:
	int expectSequenceNumberSend;	// 下一个待发送的分组序号 
	bool waitingState;				// 是否处于等待Ack的状态
	int base;                       //当前窗口基序号(最早的未确认分组的序号)
	int winlen;                     //窗口长度
	int seqlen;                     //窗口中的序号长度
	deque<Packet> window;           //窗口队列
	Packet packetWaitingAck;		//已发送并等待Ack的数据包

public:

	bool getWaitingState();
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);						//Timeout handler，将被NetworkServiceSimulator调用
	void printFrontSlideWindow(fstream& file, int RcvdAckNum);	//将滑动前窗口等内容重定向到文件
	void printRearSlideWindow(fstream& file);					//将滑动后窗口等内容重定向到文件
public:
	GBNSender();
	virtual ~GBNSender();
};

#endif