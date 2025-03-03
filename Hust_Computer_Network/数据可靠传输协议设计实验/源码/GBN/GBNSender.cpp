#include "stdafx.h"
#include "Global.h"
#include "GBNSender.h"
#include<deque>

GBNSender::GBNSender() :expectSequenceNumberSend(0), waitingState(false), base(0), winlen(4), seqlen(8)
{
}


GBNSender::~GBNSender()
{
}



bool GBNSender::getWaitingState() {
	if (window.size() == winlen)
		this->waitingState = 1;	//���ʹ������������ڵȴ�״̬
	else 
		this->waitingState = 0;	//���򲻴��ڵȴ�״̬
	return this->waitingState;
}




bool GBNSender::send(const Message& message) {
	if (this->getWaitingState()) { // �����ͷ�������ʱ���ܾ�����
		return false;
	}

	this->packetWaitingAck.acknum = -1; // ���Ը��ֶ�
	this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
	this->packetWaitingAck.checksum = 0;
	memcpy(this->packetWaitingAck.payload, message.data, sizeof(message.data));
	this->packetWaitingAck.checksum = pUtils->calculateCheckSum(this->packetWaitingAck);	// ��������
	window.push_back(packetWaitingAck);	// �������͵İ����봰�ڶ���
	pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck);

	if (this->base == this->expectSequenceNumberSend)	
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// ������͵��Ǵ�����ĵ�һ�����飬��ô��Ҫ����timer

	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck);	// ����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
	this->expectSequenceNumberSend = (this->expectSequenceNumberSend + 1) % this->seqlen;	//������һ�����

	return true;
}

void GBNSender::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);	// ���У����Ƿ���ȷ
	int offacknum = (ackPkt.acknum - this->base + this->seqlen) % this->seqlen;	// ���㴰���е�ȷ�����                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

	// ���У�����ȷ������ȷ��������ڷ��ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ��������
	if (checkSum == ackPkt.checksum && offacknum < window.size()) {
		
		// �������ǰ�Ĵ�������
		int ackNum = ackPkt.acknum;
		std::fstream outfile;
		outfile.open("GBNOutput.txt", ios::app);
		printFrontSlideWindow(outfile, ackNum);	

		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		pns->stopTimer(SENDER, this->base);   // ��baseΪ��׼���رն�ʱ��
		while (this->base != (ackPkt.acknum + 1) % this->seqlen) {	// ��������
			window.pop_front();// ���ѳɹ����յ�ACK��֮ǰ���˳�����
			this->base = (this->base + 1) % this->seqlen;	// ���»����
		}  

		// ���ճɹ���Ĵ���
		printRearSlideWindow(outfile);
		outfile.close();

		if (window.size() != 0) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// ��baseΪ��׼��������ʱ��
		}

	}
	else if (checkSum != ackPkt.checksum)
		pUtils->printPacket("���ͷ�û����ȷ�յ��ñ���ȷ��,����У�����", ackPkt);
	else
		pUtils->printPacket("���ͷ�����ȷ�յ����ñ���ȷ��", ackPkt);

}

void GBNSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum);	//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);	//�����������ͷ���ʱ��
	// ���·��ʹ����ڵ����д�ȷ�����ݰ�
	for (int i = 0; i < window.size(); i++) {
		pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط����ڱ���", window.at(i));
		pns->sendToNetworkLayer(RECEIVER, window.at(i));
	}		                                                             

}


void GBNSender::printFrontSlideWindow(fstream& file, int RcvdAckNum) {

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


void GBNSender::printRearSlideWindow(fstream& file) {
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