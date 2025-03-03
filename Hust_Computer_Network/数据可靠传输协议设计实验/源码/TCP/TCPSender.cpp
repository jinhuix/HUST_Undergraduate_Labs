#include "stdafx.h"
#include "Global.h"
#include "TCPSender.h"
#include<deque>

TCPSender::TCPSender() :expectSequenceNumberSend(0), waitingState(false), base(0), winlen(4), seqlen(8), Rdnum(0)
{
}


TCPSender::~TCPSender()
{
}



bool TCPSender::getWaitingState() {
	if (window.size() == winlen)
		this->waitingState = true;
	else this->waitingState = false;
	return this->waitingState;
}




bool TCPSender::send(const Message& message) {
	if (this->getWaitingState()) { //�����ͷ�������ʱ���ܾ�����
		return false;
	}

	this->packetWaitingAck.acknum = -1; //���Ը��ֶ�
	this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
	this->packetWaitingAck.checksum = 0;
	memcpy(this->packetWaitingAck.payload, message.data, sizeof(message.data));
	this->packetWaitingAck.checksum = pUtils->calculateCheckSum(this->packetWaitingAck);
	window.push_back(packetWaitingAck);	//�������͵İ����봰�ڶ���
	pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck);

	if (this->base == this->expectSequenceNumberSend)
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// ��baseΪ��׼��������ʱ��

	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck);	// ����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
	this->expectSequenceNumberSend = (this->expectSequenceNumberSend + 1) % this->seqlen;	// ������һ���ڴ����

	return true;
}


void TCPSender::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);	// ���У����Ƿ���ȷ
	int offacknum = (ackPkt.acknum - this->base + this->seqlen) % this->seqlen;

	//���У�����ȷ������ȷ��������ڷ��ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ��������
	if (checkSum == ackPkt.checksum && offacknum < window.size()) {

		// �������ǰ�Ĵ�������
		int ackNum = ackPkt.acknum;
		std::fstream outfile;
		outfile.open("TCPOutput.txt", ios::app);
		printFrontSlideWindow(outfile, ackNum);

		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		pns->stopTimer(SENDER, this->base);   // ��baseΪ��׼���رն�ʱ��
		// ���ѳɹ����յ�ACK��֮ǰ���˳����У������ڻ�����ACK+1��λ��
		while (this->base != (ackPkt.acknum + 1) % this->seqlen) {	
			window.pop_front();
			this->base = (this->base + 1) % this->seqlen;
		}  


		// ���ճɹ���Ĵ���
		printRearSlideWindow(outfile);
		outfile.close();

		this->Rdnum = 0;  //�յ���ȷ��ʱ���������������
		if (window.size() != 0) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// ��baseΪ��׼��������ʱ��
		}

	}
	else if (checkSum != ackPkt.checksum)
		pUtils->printPacket("���ͷ�û����ȷ�յ��ñ���ȷ��,����У�����", ackPkt);
	else if (ackPkt.acknum == (this->base + this->seqlen - 1) % this->seqlen) {
		pUtils->printPacket("���ͷ�����ȷ�յ����ñ���ȷ��", ackPkt);
		this->Rdnum++;
		// ����յ�3����������ACK�����п����ش�
		if (this->Rdnum == 3 && window.size() > 0) {
			pUtils->printPacket("���ͷ����������ش����ƣ��ش����緢����û��ȷ�ϵı��Ķ�", window.front());
			pns->sendToNetworkLayer(RECEIVER, window.front());	
			this->Rdnum = 0;	// ����ACK��������0
		}
	}

}

void TCPSender::timeoutHandler(int seqNum) {
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط����緢����û��ȷ�ϵı��Ķ�", window.front());
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	pns->sendToNetworkLayer(RECEIVER, window.front());	             //���·������緢����û��ȷ�ϵı��Ķ�
}


void TCPSender::printFrontSlideWindow(fstream& file, int RcvdAckNum) {

	cout << "rcvdAckNum: " << RcvdAckNum << endl;
	cout << "baseSeqNum: " << this->base << endl;
	cout << "nextSeqNum: " << this->expectSequenceNumberSend << endl;
	cout << "frontSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		cout << (this->base + i) % this->seqlen << " ";
	}
	cout << "]" << endl;

	file << "rcvdAckNum: " << RcvdAckNum << endl;
	file << "baseSeqNum: " << this->base << endl;
	file << "nextSeqNum: " << this->expectSequenceNumberSend << endl;
	file << "frontSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		file << (this->base + i) % this->seqlen << " ";
	}
	file << "]" << endl;
}


void TCPSender::printRearSlideWindow(fstream& file) {
	cout << "rearSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		cout << (this->base + i) % this->seqlen << " ";
	}
	cout << "]" << endl;
	cout << "=================================================================" << endl;


	file << "rearSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		file << (this->base + i) % this->seqlen << " ";
	}
	file << "]" << endl;
	file << "=================================================================" << endl;
}