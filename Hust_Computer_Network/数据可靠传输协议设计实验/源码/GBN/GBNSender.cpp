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
		this->waitingState = 1;	//发送窗口已满，则处于等待状态
	else 
		this->waitingState = 0;	//否则不处于等待状态
	return this->waitingState;
}




bool GBNSender::send(const Message& message) {
	if (this->getWaitingState()) { // 当发送方窗口满时，拒绝发送
		return false;
	}

	this->packetWaitingAck.acknum = -1; // 忽略该字段
	this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
	this->packetWaitingAck.checksum = 0;
	memcpy(this->packetWaitingAck.payload, message.data, sizeof(message.data));
	this->packetWaitingAck.checksum = pUtils->calculateCheckSum(this->packetWaitingAck);	// 计算检验和
	window.push_back(packetWaitingAck);	// 将待发送的包加入窗口队列
	pUtils->printPacket("发送方发送报文", this->packetWaitingAck);

	if (this->base == this->expectSequenceNumberSend)	
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// 如果发送的是窗口里的第一个分组，那么需要启动timer

	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck);	// 调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
	this->expectSequenceNumberSend = (this->expectSequenceNumberSend + 1) % this->seqlen;	//更新下一个序号

	return true;
}

void GBNSender::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);	// 检查校验和是否正确
	int offacknum = (ackPkt.acknum - this->base + this->seqlen) % this->seqlen;	// 计算窗口中的确认序号                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

	// 如果校验和正确，并且确认序号是在发送方已发送并等待确认的数据包序号列中
	if (checkSum == ackPkt.checksum && offacknum < window.size()) {
		
		// 输出滑动前的窗口序列
		int ackNum = ackPkt.acknum;
		std::fstream outfile;
		outfile.open("GBNOutput.txt", ios::app);
		printFrontSlideWindow(outfile, ackNum);	

		pUtils->printPacket("发送方正确收到确认", ackPkt);
		pns->stopTimer(SENDER, this->base);   // 以base为基准，关闭定时器
		while (this->base != (ackPkt.acknum + 1) % this->seqlen) {	// 滑动窗口
			window.pop_front();// 将已成功接收的ACK及之前的退出队列
			this->base = (this->base + 1) % this->seqlen;	// 更新基序号
		}  

		// 接收成功后的窗口
		printRearSlideWindow(outfile);
		outfile.close();

		if (window.size() != 0) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// 以base为基准，开启定时器
		}

	}
	else if (checkSum != ackPkt.checksum)
		pUtils->printPacket("发送方没有正确收到该报文确认,数据校验错误", ackPkt);
	else
		pUtils->printPacket("发送方已正确收到过该报文确认", ackPkt);

}

void GBNSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum);	//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);	//重新启动发送方定时器
	// 重新发送窗口内的所有待确认数据包
	for (int i = 0; i < window.size(); i++) {
		pUtils->printPacket("发送方定时器时间到，重发窗口报文", window.at(i));
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