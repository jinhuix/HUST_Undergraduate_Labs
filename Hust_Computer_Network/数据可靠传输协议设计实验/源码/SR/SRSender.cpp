#include "stdafx.h"
#include "Global.h"
#include "SRSender.h"


SRSender::SRSender() :expectSequenceNumberSend(0), waitingState(false), base(0), winlen(4), seqlen(8)
{
}


SRSender::~SRSender()
{
}



bool SRSender::getWaitingState() {
	if (window.size() == winlen)
		this->waitingState = 1;
	else this->waitingState = 0;
	return this->waitingState;
}




bool SRSender::send(const Message& message) {
	if (this->getWaitingState()) { //�����ͷ�������ʱ���ܾ�����
		return false;
	}

	this->packetWaitingAck.acknum = -1; //���Ը��ֶ�
	this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
	this->packetWaitingAck.checksum = 0;
	memcpy(this->packetWaitingAck.payload, message.data, sizeof(message.data));
	this->packetWaitingAck.checksum = pUtils->calculateCheckSum(this->packetWaitingAck);	//��������
	waitPck tempPck;
	tempPck.flag = false;
	tempPck.winPck = packetWaitingAck;
	window.push_back(tempPck);          //�������Ҵ�ȷ�ϵİ����봰�ڶ���
	pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck);

	pns->startTimer(SENDER, Configuration::TIME_OUT, this->expectSequenceNumberSend);	//�������ͷ���ʱ������ʱÿ����һ����������Ӧ�Ķ�ʱ��

	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
	this->expectSequenceNumberSend = (this->expectSequenceNumberSend + 1) % this->seqlen;	//������һ�����

	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);	// ���У����Ƿ���ȷ
	int offseqnum = (ackPkt.acknum - this->base + this->seqlen) % this->seqlen;	// ȷ�ϵ����ݰ��ڴ����е����
	// ���У�����ȷ������ȷ��������ڷ��ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ��������
	if (checkSum == ackPkt.checksum && offseqnum < window.size() && window.at(offseqnum).flag == false) {
		window.at(offseqnum).flag = true;	// ����յ�ack
		pns->stopTimer(SENDER, ackPkt.acknum);   // �رն�Ӧ���ݰ��Ķ�ʱ��

		// ��ӡ����ǰ�Ĵ�������
		int ackNum = ackPkt.acknum;
		std::fstream outfile;
		outfile.open("SROutput.txt", ios::app);
		printFrontSlideWindow(outfile, ackNum);	// �������ACKǰ�Ĵ�������
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);

		// �����ڻ�����û���յ�����ACK��λ��
		while (window.size() != 0 && window.front().flag == true) {	
			window.pop_front();
			this->base = (this->base + 1) % this->seqlen;
		}  

		// ��ӡ������Ĵ�������
		printRearSlideWindow(outfile);
		outfile.close();

	}
	else if (checkSum != ackPkt.checksum)
		pUtils->printPacket("���ͷ�û����ȷ�յ��ñ���ȷ��,����У�����", ackPkt);
	else
		pUtils->printPacket("���ͷ�����ȷ�յ����ñ���ȷ��", ackPkt);
}

void SRSender::timeoutHandler(int seqNum) {
	int offseqnum = (seqNum - this->base + this->seqlen) % this->seqlen;
	pns->stopTimer(SENDER, seqNum);  									//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	pns->sendToNetworkLayer(RECEIVER, window.at(offseqnum).winPck);
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", window.at(offseqnum).winPck);
}


void SRSender::printFrontSlideWindow(fstream& file, int RcvdAckNum) {
	cout << "���ͷ���������:" << endl;
	cout << "rcvdAckNum: " << RcvdAckNum << endl;
	cout << "baseSeqNum: " << this->base << endl;
	cout << "nextSeqNum: " << this->expectSequenceNumberSend << endl;
	cout << "frontSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		if (i < window.size()) {
			if (window.at(i).flag == true)
				cout << (this->base + i) % seqlen << "Y ";
			else cout << (this->base + i) % seqlen << "N ";
		}
		else cout << (this->base + i) % seqlen << " ";
	}
	cout << "]" << endl;

	file << "���ͷ���������:" << endl;
	file << "rcvdAckNum: " << RcvdAckNum << endl;
	file << "baseSeqNum: " << this->base << endl;
	file << "nextSeqNum: " << this->expectSequenceNumberSend << endl;
	file << "frontSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		if (i < window.size()) {
			if (window.at(i).flag == true)
				file << (this->base + i) % seqlen << "Y ";
			else file << (this->base + i) % seqlen << "N ";
		}
		else file << (this->base + i) % seqlen << " ";
	}
	file << "]" << endl;
}


void SRSender::printRearSlideWindow(fstream& file) {
	cout << "rearSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		if (i < window.size()) {
			if (window.at(i).flag == true)
				cout << (this->base + i) % seqlen << "Y ";
			else cout << (this->base + i) % seqlen << "N ";
		}
		else cout << (this->base + i) % seqlen << " ";
	}
	cout << "]" << endl;
	cout << "=================================================================" << endl;


	file << "rearSlidingWindow: [ ";
	for (int i = 0; i < this->winlen; i++) {
		if (i < window.size()) {
			if (window.at(i).flag == true)
				file << (this->base + i) % seqlen << "Y ";
			else file << (this->base + i) % seqlen << "N ";
		}
		else file << (this->base + i) % seqlen << " ";
	}
	file << "]" << endl;
	file << "=================================================================" << endl;
}