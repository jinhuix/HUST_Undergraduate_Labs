#ifndef STOP_WAIT_RDT_SENDER_H
#define STOP_WAIT_RDT_SENDER_H
#include "RdtSender.h"
#include<deque>

class GBNSender :public RdtSender
{
private:
	int expectSequenceNumberSend;	// ��һ�������͵ķ������ 
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	int base;                       //��ǰ���ڻ����(�����δȷ�Ϸ�������)
	int winlen;                     //���ڳ���
	int seqlen;                     //�����е���ų���
	deque<Packet> window;           //���ڶ���
	Packet packetWaitingAck;		//�ѷ��Ͳ��ȴ�Ack�����ݰ�

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);						//Timeout handler������NetworkServiceSimulator����
	void printFrontSlideWindow(fstream& file, int RcvdAckNum);	//������ǰ���ڵ������ض����ļ�
	void printRearSlideWindow(fstream& file);					//�������󴰿ڵ������ض����ļ�
public:
	GBNSender();
	virtual ~GBNSender();
};

#endif