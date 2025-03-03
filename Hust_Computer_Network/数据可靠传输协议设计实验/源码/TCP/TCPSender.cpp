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
	if (this->getWaitingState()) { //当发送方窗口满时，拒绝发送
		return false;
	}

	this->packetWaitingAck.acknum = -1; //忽略该字段
	this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
	this->packetWaitingAck.checksum = 0;
	memcpy(this->packetWaitingAck.payload, message.data, sizeof(message.data));
	this->packetWaitingAck.checksum = pUtils->calculateCheckSum(this->packetWaitingAck);
	window.push_back(packetWaitingAck);	//将待发送的包加入窗口队列
	pUtils->printPacket("发送方发送报文", this->packetWaitingAck);

	if (this->base == this->expectSequenceNumberSend)
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// 以base为基准，开启定时器

	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck);	// 调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
	this->expectSequenceNumberSend = (this->expectSequenceNumberSend + 1) % this->seqlen;	// 更新下一个期待序号

	return true;
}


void TCPSender::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);	// 检查校验和是否正确
	int offacknum = (ackPkt.acknum - this->base + this->seqlen) % this->seqlen;

	//如果校验和正确，并且确认序号是在发送方已发送并等待确认的数据包序号列中
	if (checkSum == ackPkt.checksum && offacknum < window.size()) {

		// 输出滑动前的窗口序列
		int ackNum = ackPkt.acknum;
		std::fstream outfile;
		outfile.open("TCPOutput.txt", ios::app);
		printFrontSlideWindow(outfile, ackNum);

		pUtils->printPacket("发送方正确收到确认", ackPkt);
		pns->stopTimer(SENDER, this->base);   // 以base为基准，关闭定时器
		// 将已成功接收的ACK及之前的退出队列，将窗口滑动到ACK+1的位置
		while (this->base != (ackPkt.acknum + 1) % this->seqlen) {	
			window.pop_front();
			this->base = (this->base + 1) % this->seqlen;
		}  


		// 接收成功后的窗口
		printRearSlideWindow(outfile);
		outfile.close();

		this->Rdnum = 0;  //收到正确包时，将冗余记数重置
		if (window.size() != 0) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	// 以base为基准，开启定时器
		}

	}
	else if (checkSum != ackPkt.checksum)
		pUtils->printPacket("发送方没有正确收到该报文确认,数据校验错误", ackPkt);
	else if (ackPkt.acknum == (this->base + this->seqlen - 1) % this->seqlen) {
		pUtils->printPacket("发送方已正确收到过该报文确认", ackPkt);
		this->Rdnum++;
		// 如果收到3个以上冗余ACK，进行快速重传
		if (this->Rdnum == 3 && window.size() > 0) {
			pUtils->printPacket("发送方启动快速重传机制，重传最早发送且没被确认的报文段", window.front());
			pns->sendToNetworkLayer(RECEIVER, window.front());	
			this->Rdnum = 0;	// 冗余ACK的数量清0
		}
	}

}

void TCPSender::timeoutHandler(int seqNum) {
	pUtils->printPacket("发送方定时器时间到，重发最早发送且没被确认的报文段", window.front());
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	pns->sendToNetworkLayer(RECEIVER, window.front());	             //重新发送最早发送且没被确认的报文段
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