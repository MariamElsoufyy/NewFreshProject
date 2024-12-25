#include "node.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
using namespace std;
Define_Module(Node);


#define MAX_SEQ (par("WS").intValue() + 1) // WS+1
typedef enum
{
    frame_arrival,
    timeout,
} event_type;


void Node:: logFrameEvent(double time, int node_id, int actiontype,
                   int seq_num,  string payload, const string trailer,
                   int modified_bit,bool lostbool, int duplicate_version, double delay_interval) {
    std::ofstream output_file("output.txt", std::ios::app);
    if (!output_file) {
        std::cerr << "Error: Could not open output.txt for writing." << std::endl;
        return;
    }
    string action,lost;
    if(actiontype ==1){
        action ="sent";
    }
    else{
        action = "received";
    }

   if(lostbool){

       lost = "yes";
   }
   else
   {
       lost = "no";
   }

    // Print the formatted message
   output_file << "At time [" << time << "], Node[" << node_id << "] [" << action
              << "] frame with seq_num=[" << seq_num << "] and payload=[" << payload 
              << "] and trailer=[" << trailer << "], Modified [" << modified_bit 
              << "], Lost [" << lost << "], Duplicate [" << duplicate_version 
              << "], Delay [" << delay_interval << "]" << endl;
   output_file.close();

}

void Node:: writeToOutputFile(int node_id, bitset<4> error_code) {


    std::ofstream output_file("output.txt", std::ios::app);
    if (!output_file) {
        std::cerr << "Error: Could not open output.txt for writing." << std::endl;
        return;
    }

    output_file << "At time [" << simTime() << "], Node[" << node_id
                << "], Introducing channel error with code =[" << error_code << "].\n";

}
string Node::FrameAndFlag(string msg)
{

    char flag = '$';
    char ESC = '/';
    string currentmsg_string = "$";
    for (int i = 0; i < msg.size(); i++)
    {
        if (msg[i] == '$' || msg[i] == '/')
        {

            currentmsg_string = currentmsg_string + ESC;
        }

        currentmsg_string = currentmsg_string + msg[i];
        // EV << i << " " << currentmsg_string << endl;
    }

    currentmsg_string = currentmsg_string + flag;

    return currentmsg_string;
}

int Node::ModifyBit(string &bitstring)
{
    int rand = uniform(0, bitstring.size() - 1);

    if (bitstring[rand] == '1')
    {
        bitstring[rand] = '0';
    }
    else
    {
        bitstring[rand] = '1';
    }
    return rand ;
}
bool Node::between(int a, int b, int c)
{
    /* Return true if a <= b < c circularly; false otherwise. */
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return true;
    else
        return false;
}
char binaryToChar(const std::string &binary)
{
char binaryToChar(const std::string &binary)
{
    // Convert binary string to decimal and then to a character
    return static_cast<char>(std::bitset<8>(binary).to_ulong());
}
bitset<8> Node::CalculateParityRec(string Data, string &str)
{
    std::bitset<8> parity = 0000000;
    for (int i = 0; i < Data.size(); i = i + 8)
    {
        std::bitset<8> bits(Data.substr(i, 8));
        str = str + binaryToChar(Data.substr(i, 8));
        parity = parity ^ bits;
    }
    return parity;
}
bitset<8> Node::CalculateParity(string Data, string &bitstring)
{
    std::bitset<8> parity = 0000000;
    for (int i = 0; i < Data.size(); i++)
    {
        std::bitset<8> bits(Data[i]);
        parity = parity ^ bits;
        bitstring = bitstring + bits.to_string();
    }
    return parity;
}
void Node::CreateError(string &bitstring, bitset<4> currentmsg_bits, int & modifiedbit, bool & losssignal )
{
    DelayValue =par("PT").doubleValue() + par("TD").doubleValue();
    EV<<"gowa"<<DelayValue<<endl;
    if (!strcmp(currentmsg_bits.to_string().c_str(), "0000")) // NO ERROR
    {
        return;
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0001")) // Delay Error
    {

        DelayValue = DelayValue + par("ED").doubleValue();
        EV<<"gowa awy "<<DelayValue<<endl;
        return;
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0011")) // Delay and duplication Error
    {

        DelayValue = DelayValue + par("ED").doubleValue();
        dupDelay =  par("DD").doubleValue();
        return;
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0010")) // duplication Error
    {
        losssignal = 1;
        return;

    }
    else if ((!strcmp(currentmsg_bits.to_string().c_str(), "0100")) || (!strcmp(currentmsg_bits.to_string().c_str(), "0101"))) // Loss Error & loss and delay
    {
        DelayValue = par("PT").doubleValue();
        //////LOSSSSSSSSSSSSSSSSSSS
        losssignal = 1;
        return;
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0110")) // loss + dup
    {
        //////LOSSSSSSSSSSSSSSSSSSS
        dupDelay =  par("DD").doubleValue();
        DelayValue = par("PT").doubleValue();
        losssignal = 1;
        return;
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0111")) // loss + dup + delay ||
        {
        DelayValue = par("PT").doubleValue();
        dupDelay =  par("DD").doubleValue();

        losssignal = 1;
        return;
        }
    else if (currentmsg_bits[3] == 1) // Modification Error
    {

       modifiedbit =  ModifyBit(bitstring);

        if (!strcmp(currentmsg_bits.to_string().c_str(), "1000")) // Modification only
        {
            return;
        }

        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1001")) // Delay + modification  Error
        {

            dupDelay =  par("DD").doubleValue();
            return;
        }
        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1010")) // Modification and Duplication
        {

            dupDelay =  par("DD").doubleValue();
            return;
        }
        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1011")) // Modification and Duplication +delay
        {

            DelayValue = DelayValue + par("ED").doubleValue();
            dupDelay =  par("DD").doubleValue();
            return;
        }
        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1100")) // Modification and loss
        {        DelayValue = par("PT").doubleValue();
            losssignal = 1;
            return;
        }

        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1101")) // modification + loss + delay
        {

            /////lossssssssssssssssssssssssssssssssssssss
            DelayValue = par("PT").doubleValue();
            losssignal = 1;
            return;

        }
        else if ((!strcmp(currentmsg_bits.to_string().c_str(), "1110")) || (!strcmp(currentmsg_bits.to_string().c_str(), "1111"))) // Modification and loss  +duplication || modification + loss + duplication + delay
        {

            DelayValue = par("PT").doubleValue();
            dupDelay =  par("DD").doubleValue();
            losssignal = 1;
            return;
        }
    }
}

void Node::readInputFile(vector<MessageData> &msgs, string filename)
{
    string line;

    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        EV << "Error in opening input file in node" << endl;
        return;
    }
    while (getline(inputFile, line))
    {
        string pre = line.substr(0, 4);
        MessageData m;
        m.prefix = bitset<4>(pre);
        //////// prefix is written from right to left 4<------0
        // prefix[0] -> Delay
        // prefix[1] -> duplication
        // prefix[2] -> Loss
        // prefix[3] -> Modification
        int i = 5;
        while (line[i] == ' ')
        {
            i++;
        }
        m.data = line.substr(i);
        msgs.push_back(m);
        writeToOutputFile(node_id,m.prefix) ;
    }
}

void Node::initialize()
{
    frame_expected = 0;
    window_size = par("WS").intValue();           // Set window size from .ini file
    send_window.resize(window_size, nullptr);     // Allocate buffer
    timeout_event = new cMessage("TimeoutEvent"); // Create timeout event
}
void Node::handleMessage(cMessage *msg)
{
    DelayValue = 0;
    dupDelay=0;

     if (msg->getArrivalGateId() == 0) /// msg from coordinator
    {


        if (!strcmp(this->getName(), "node0"))
        {
            filename = "input0.txt";
            node_id = 0;
        }
        else if (!strcmp(this->getName(), "node1"))
        {
            filename = "input1.txt";
            node_id = 1;
        }
        readInputFile(msgs, filename);
        int counter = 0;
        while (counter < window_size && !msgs.empty())
        {
            bool losssignal = 0;
            NodeMessage_Base *frame_dup,*frame = new NodeMessage_Base("");
            MessageData currentMsg = msgs[counter];
            int seq_num = counter % (MAX_SEQ + 1); // Sequence number wraps around
            frame->setHeader(seq_num);             // Add sequence number
            frame->setFrame_Type(2);
            string currentmsg_data = currentMsg.data;
            bitset<4> errorBits = currentMsg.prefix;

            /////// Framing //////
            string currentmsg_framed = FrameAndFlag(currentmsg_data);
            /////// Parity //////
            string bitstring = "";
            bitset<8> parity = CalculateParity(currentmsg_framed, bitstring);
            frame->setTrailer(parity.to_string().c_str());
            //////setting payload////////
            int modifiedbit = -1;
            CreateError(bitstring,currentMsg.prefix, modifiedbit , losssignal);
            EV<<"bara"<<DelayValue<<endl;
            frame->setM_Payload(bitstring.c_str());
            counter++;
            totalDelay= totalDelay + DelayValue;
            int dupversion =-1;
            double delay_interval=0;
            if(dupDelay!=0){
                frame_dup = frame->dup();
                dupversion = 1;
            }
            if(currentMsg.prefix[0]==1){
                delay_interval =  par("ED").doubleValue();
            }
            if(losssignal)
            {
                logFrameEvent(totalDelay+static_cast<double>(simTime().inUnit(SIMTIME_S)),node_id, 1,
                                               seq_num,  bitstring, frame->getTrailer(),
                                               modifiedbit,losssignal, dupversion,  delay_interval);
                if(dupversion ==1){
                    dupversion =2;
                    logFrameEvent(totalDelay+static_cast<double>(simTime().inUnit(SIMTIME_S)),node_id, 1,
                                                                seq_num,  bitstring, frame->getTrailer(),
                                                                modifiedbit,losssignal, dupversion,  delay_interval);
                }
            }
            else {
            sendDelayed(frame, totalDelay, "node$o"); // Send frame
            logFrameEvent(totalDelay+static_cast<double>(simTime().inUnit(SIMTIME_S)),node_id, 1,
                                seq_num,  bitstring, frame->getTrailer(),
                                modifiedbit,losssignal, dupversion,  delay_interval);
            if(dupDelay!=0){
                dupversion =2;
                        totalDelay= totalDelay + dupDelay;
            sendDelayed(frame_dup, totalDelay, "node$o"); // Duplication
            logFrameEvent(totalDelay+static_cast<double>(simTime().inUnit(SIMTIME_S)),node_id, 1,
                                seq_num,  bitstring, frame_dup->getTrailer(),
                                modifiedbit,losssignal, dupversion,  delay_interval);
            }
            DelayValue = 0;
            dupDelay=0;
            }
        }
    }
    else
    {
        NodeMessage_Base *ReceivedMessage = check_and_cast<NodeMessage_Base *>(msg);
        if (ReceivedMessage->getFrame_Type() == 2) // recieving data [reciever]
        {
            int seqnum = ReceivedMessage->getHeader();
            string payload = ReceivedMessage->getM_Payload(); // Extract payload
            EV << payload << endl;
            string trailer = ReceivedMessage->getTrailer(); // Extract trailer
            string str = "";
            std::bitset<8> parity_check = CalculateParityRec(payload, str);
            NodeMessage_Base *ack_nack = new NodeMessage_Base("msg");

            if (strcmp(parity_check.to_string().c_str(), trailer.c_str()))
            {
                // There is an error detected so send NACK
                ack_nack->setHeader(seqnum); // NACK for the received sequence number
                ack_nack->setM_Payload("");  // No payload needed for NACK
                ack_nack->setFrame_Type(0);  // Type for NACK can be set to 0
                ack_nack->setACK(ReceivedMessage->getHeader());
            }
            else // correct frame sent in order
            {
                frame_expected = (frame_expected + 1) % (MAX_SEQ + 1); // Update frame_expected to the next sequence number and wrap around
                ack_nack->setHeader(seqnum);                           // ACK for the current sequence number
                ack_nack->setM_Payload("");
                ack_nack->setFrame_Type(1); // Type for ACK can be set to 1
                int expected = 0;
                if (expected == ReceivedMessage->getHeader())
                {
                    expected++;
                    if (expected == MAX_SEQ)
                        expected = 0;
                }
                ack_nack->setACK((expected + MAX_SEQ - 1) % (MAX_SEQ));

                /// Process the payload
                string deframed_payload = "";
                bool escape_next = false;
                string ESC = "00011011";
                string s = "";
                // Start deframing
                for (int i = 8; i < payload.size() - 9; i = i + 8) // Skip the first and last characters (flags)
                {
                    if (escape_next)
                    {
                        // Handle escaped character
                        deframed_payload += payload.substr(i, 8);
                        escape_next = false;
                    }
                    else if (!strcmp(payload.substr(i, 8).c_str(), ESC.c_str()))
                    {
                        // Set flag to escape the next character
                        escape_next = true;
                    }
                    else
                    {
                        // Normal character
                        deframed_payload += payload.substr(i, 8);
                        int asciiCode = std::bitset<8>(payload.substr(i, 8)).to_ulong();
                        char letter = (char)asciiCode;
                        s = s + letter;
                    }
                }

                EV << "Deframed payload: " << deframed_payload << endl;
                EV << "Text " << s;
            }

            sendDelayed(ack_nack, par("TD").doubleValue() + par("PT").doubleValue(), "node$o");
        }
        //         else if (ReceivedMessage->getFrame_Type() == 1) // recieving Ack [sender]
        //         {
        //             int ack_num = ReceivedMessage->getHeader(); // Get ACK number
        //             EV << "Received ACK for frame " << ack_num << endl;
        //
        //             // Slide the window
        //             while (ack_expected != (ack_num + 1) % (MAX_SEQ + 1))
        //             {
        //                 delete send_window[ack_expected % window_size]; // Free frame memory
        //                 send_window[ack_expected % window_size] = nullptr;
        //                 ack_expected = (ack_expected + 1) % (MAX_SEQ + 1); // Move the window
        //             }
        //
        //             // Stop timer if all frames are acknowledged
        //             if (ack_expected == next_frame_to_send)
        //             {
        //                 cancelEvent(timeout_event);
        //             }
        //         }
        //         else if (ReceivedMessage->getFrame_Type() == 0) // recieving NACK [sender]
        //         {
        //             int nack_num = ReceivedMessage->getHeader(); // Get NACK number
        //             EV << "Received NACK for frame " << nack_num << endl;
        //
        //             // Resend the frame
        //             sendDelayed(send_window[nack_num % window_size]->dup(), par("PT").doubleValue(), "out");
        //         }
        //     }
    }
}
