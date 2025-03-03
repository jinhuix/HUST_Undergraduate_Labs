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
	if (this->getWaitingState()) { //当发送方窗口满时，拒绝发送
		return false;
	}

	this->packetWaitingAck.acknum = -1; //忽略该字段
	this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
	this->packetWaitingAck.checksum = 0;
	memcpy(this->packetWaitingAck.payload, message.data, sizeof(message.data));
	this->packetWaitingAck.checksum = pUtils->calculateCheckSum(this->packetWaitingAck);	//计算检验和
	waitPck tempPck;
	tempPck.flag = false;
	tempPck.winPck = packetWaitingAck;
	window.push_back(tempPck);          //将发送且待确认的包加入窗口队列
	pUtils->printPacket("发送方发送报文", this->packetWaitingAck);

	pns->startTimer(SENDER, Configuration::TIME_OUT, this->expectSequenceNumberSend);	//启动发送方定时器，此时每发送一个包启动对应的定时器

	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
	this->expectSequenceNumberSend = (this->expectSequenceNumberSend + 1) % this->seqlen;	//更新下一个序号

	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);	// 检查校验和是否正确
	int offseqnum = (ackPkt.acknum - this->base + this->seqlen) % this->seqlen;	// 确认的数据包在窗口中的序号
	// 如果校验和正确，并且确认序号是在发送方已发送并等待确认的数据包序号列中
	if (checkSum == ackPkt.checksum && offseqnum < window.size() && window.at(offseqnum).flag == false) {
		window.at(offseqnum).flag = true;	// 标记收到ack
		pns->stopTimer(SENDER, ackPkt.acknum);   // 关闭对应数据包的定时器

		// 打印滑动前的窗口序列
		int ackNum = ackPkt.acknum;
		std::fstream outfile;
		outfile.open("SROutput.txt", ios::app);
		printFrontSlideWindow(outfile, ackNum);	// 输出接收ACK前的窗口序列
		pUtils->printPacket("发送方正确收到确认", ackPkt);

		// 将窗口滑动至没有收到报文ACK的位置
		while (window.size() != 0 && window.front().flag == true) {	
			window.pop_front();
			this->base = (this->base + 1) % this->seqlen;
		}  

		// 打印滑动后的窗口序列
		printRearSlideWindow(outfile);
		outfile.close();

	}
	else if (checkSum != ackPkt.checksum)
		pUtils->printPacket("发送方没有正确收到该报文确认,数据校验错误", ackPkt);
	else
		pUtils->printPacket("发送方已正确收到过该报文确认", ackPkt);
}

void SRSender::timeoutHandler(int seqNum) {
	int offseqnum = (seqNum - this->base + this->seqlen) % this->seqlen;
	pns->stopTimer(SENDER, seqNum);  									//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	pns->sendToNetworkLayer(RECEIVER, window.at(offseqnum).winPck);
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", window.at(offseqnum).winPck);
}


void SRSender::printFrontSlideWindow(fstream& file, int RcvdAckNum) {
	cout << "发送方窗口详情:" << endl;
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

	file << "发送方窗口详情:" << endl;
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