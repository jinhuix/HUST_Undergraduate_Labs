#ifndef STOP_WAIT_RDT_SENDER_H
#define STOP_WAIT_RDT_SENDER_H
#include "RdtSender.h"
#include <deque>
struct waitPck {
	bool flag;     //当未接收到ACK时，为false
	Packet winPck;
};

class SRSender :public RdtSender
{
private:
	int expectSequenceNumberSend;	// 下一个发送序号 
	bool waitingState;				// 是否处于等待Ack的状态
	int base;                       //当前窗口基序号
	int winlen;                     //窗口大小
	int seqlen;                     //序号宽度
	deque<waitPck> window;           //窗口队列
	Packet packetWaitingAck;		//已发送并等待Ack的数据包

public:

	bool getWaitingState();
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
	void printFrontSlideWindow(fstream& file, int RcvdAckNum);
	void printRearSlideWindow(fstream& file);

public:
	SRSender();
	virtual ~SRSender();
};


#endif