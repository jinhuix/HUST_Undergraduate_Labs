#include "stdafx.h"
#include "Global.h"
#include "SRReceiver.h"


SRReceiver::SRReceiver() :expectSequenceNumberRcvd(0), seqlen(8), base(0), winlen(4)
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	//����������������Ϊ�հ���flag��Ϊfalse�������������
	for (int i = 0; i < winlen; i++) {
		rcvPck blank;
		blank.flag = false;
		blank.winPck.seqnum = -1;
		window.push_back(blank);
	}                                

}


SRReceiver::~SRReceiver()
{
}

void SRReceiver::receive(const Packet& packet) {
	int checkSum = pUtils->calculateCheckSum(packet);	// ���У����Ƿ���ȷ
	int offseqnum = (packet.seqnum - this->base + this->seqlen) % this->seqlen;	// �����ݰ��ڴ����е����

	//���У�����ȷ��ͬʱ�յ����ĵ�����ڽ��մ��ڷ�Χ��
	if (checkSum == packet.checksum && offseqnum < this->winlen && window.at(offseqnum).flag == false) {
		window.at(offseqnum).flag = true;	// ���ô���״̬����Ϊture��ռ��
		window.at(offseqnum).winPck = packet;

		// �������ǰ��������
		std::fstream outfile;
		outfile.open("SROutput.txt", ios::app);
		printFrontSlideWindow(outfile);
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

		while (window.front().flag == true) {
			Message msg;
			memcpy(msg.data, window.front().winPck.payload, sizeof(window.front().winPck.payload));
			pns->delivertoAppLayer(RECEIVER, msg);
			this->base = (this->base + 1) % seqlen;
			rcvPck blank;
			blank.flag = false;
			blank.winPck.seqnum = -1;
			window.pop_front();	// ����ȷ�ϵ����ݰ��Ƴ�����
			window.push_back(blank); // �������һ���һ��
		}

		// ���������Ĵ�������
		printRearSlideWindow(outfile);
		outfile.close();

		lastAckPkt.acknum = packet.seqnum; // ȷ����ŵ����յ��ı������
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	// ����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�


	}
	else {
		if (checkSum != packet.checksum) {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		}
		else {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������ȷ��", packet);
			lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢����ȷ�ϱ��ĵ�ACK
		}
	}
}


void SRReceiver::printFrontSlideWindow(fstream& file) {
	cout << "���շ���������:" << endl;
	cout << "frontSlidingWindow: [ ";
	for (int i = 0; i < window.size(); i++) {
		if (window.at(i).flag == true)
			cout << (this->base + i) % seqlen << "Y ";
		else cout << (this->base + i) % seqlen << "N ";
	}
	cout << "]" << endl;

	file << "���շ���������:" << endl;
	file << "frontSlidingWindow: [ ";
	for (int i = 0; i < window.size(); i++) {
		if (window.at(i).flag == true)
			file << (this->base + i) % seqlen << "Y ";
		else file << (this->base + i) % seqlen << "N ";
	}
	file << "]" << endl;
}


void SRReceiver::printRearSlideWindow(fstream& file) {
	cout << "rearSlidingWindow: [ ";
	for (int i = 0; i < window.size(); i++) {
		if (window.at(i).flag == true)
			cout << (this->base + i) % seqlen << "Y ";
		else cout << (this->base + i) % seqlen << "N ";
	}
	cout << "]" << endl;
	cout << "=================================================================" << endl;
	

	file << "rearSlidingWindow: [ ";
	for (int i = 0; i < window.size(); i++) {
		if (window.at(i).flag == true)
			file << (this->base + i) % seqlen << "Y ";
		else file << (this->base + i) % seqlen << "N ";
	}
	file << "]" << endl;
	file << "=================================================================" << endl;
}

