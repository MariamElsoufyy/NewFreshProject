//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __NETWORKSPROJECT_NODE_H_
#define __NETWORKSPROJECT_NODE_H_


#include <omnetpp.h>
#include <string>
#include<bitset>
#include<vector>
#include <iostream>
#include <fstream>
#include "NodeMessage_m.h"
using namespace omnetpp;
using namespace std;

struct MessageData{

    string data;
    bitset<4> prefix;  ////////written from right to left 4<------0
                                  //prefix[0] -> Delay
                                  //prefix[1] -> duplication
                                  //prefix[2] -> Loss
                                  //prefix[3] -> Modification
};

class Node : public cSimpleModule
{


private:
    string filename;
    int node_id = -1 ;
    vector<MessageData> msgs;
    int frame_expected = 0;         // Receiver's expected frame sequence number
    int ack_expected = 0;
    int next_frame_to_send = 0;
    int nbuffered = 0;
    int window_size = 0;
    double DelayValue = 0;
    double dupDelay = 0;
    int lossCounter = -1;
    double totalDelay = 0;

    std::vector<NodeMessage_Base*> send_window; // Buffer for unacknowledged frames
    cMessage * timeout_event; // Timeout event

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    string FrameAndFlag(string msg);
   int ModifyBit(string &bitstring);
   void CreateError(string &bitstring, bitset<4> currentmsg_bits,int & modifiedbit, bool & losssignal );
   bitset<8> CalculateParity(string Data, string &bitstring);
   bitset<8> CalculateParityRec(string Data, string &str);
   bool between(int a, int b, int c);
    void readInputFile(vector<MessageData>&msgs,string filename);
   void writeToOutputFile( int node_id, bitset<4> error_code);
   void logFrameEvent(double time, int node_id, int actiontype,
                      int seq_num,  string payload, const string trailer,
                      int modified_bit,bool lostbool, int duplicate_version, double delay_interval);
};

#endif
