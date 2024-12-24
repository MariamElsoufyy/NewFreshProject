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
void Node::ModifyBit(string &bitstring)
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
    // Convert binary string to decimal and then to a character
    return static_cast<char>(std::bitset<8>(binary).to_ulong());
}
bitset<8> Node::CalculateParityRec(string Data, string &str, bitset<8> trailer)
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
void Node::CreateError(string &bitstring, bitset<4> currentmsg_bits, NodeMessage_Base *nodemsg, NodeMessage_Base *nodemsg2, int &DelayValue, int &dupDelay)
{
    if (!strcmp(currentmsg_bits.to_string().c_str(), "0000")) // NO ERROR
    {

        nodemsg->setM_Payload(bitstring.c_str());
        DelayValue = par("PT").doubleValue() + par("TD").doubleValue();
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0001")) // Delay Error
    {

        nodemsg->setM_Payload(bitstring.c_str());
        DelayValue = par("PT").doubleValue() + par("TD").doubleValue() + par("ED").doubleValue();
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0011")) // Delay and duplication Error
    {

        nodemsg->setM_Payload(bitstring.c_str());
        nodemsg2->setM_Payload(nodemsg->getM_Payload());
        nodemsg2->setTrailer(nodemsg->getTrailer());
        DelayValue = par("PT").doubleValue() + par("TD").doubleValue() + par("ED").doubleValue();
        dupDelay = par("PT").doubleValue() + par("TD").doubleValue() + par("ED").doubleValue() + par("DD").doubleValue();
    }
    else if (!strcmp(currentmsg_bits.to_string().c_str(), "0010")) // duplication Error
    {

        nodemsg->setM_Payload(bitstring.c_str());

        nodemsg2->setM_Payload(nodemsg->getM_Payload());
        nodemsg2->setTrailer(nodemsg->getTrailer());

        DelayValue = par("PT").doubleValue() + par("TD").doubleValue();
        dupDelay = par("PT").doubleValue() + par("TD").doubleValue() + par("DD").doubleValue();
    }
    else if ((!strcmp(currentmsg_bits.to_string().c_str(), "0100")) || (!strcmp(currentmsg_bits.to_string().c_str(), "0101"))) // Loss Error & loss and delay
    {

        nodemsg->setM_Payload(bitstring.c_str());
        scheduleAt(simTime() + par("PT").doubleValue(), new cMessage("lost"));
    }
    else if ((!strcmp(currentmsg_bits.to_string().c_str(), "0110")) || (!strcmp(currentmsg_bits.to_string().c_str(), "0111"))) // loss + dup + delay || loss + dup
    {

        nodemsg->setM_Payload(bitstring.c_str());

        nodemsg2->setM_Payload(nodemsg->getM_Payload());
        nodemsg2->setTrailer(nodemsg->getTrailer());
        scheduleAt(simTime() + par("PT").doubleValue(), new cMessage("lost"));
        // scheduleAt(simTime() + par("PT").doubleValue(), new cMessage("lost"));
    }
    else if (currentmsg_bits[3] == 1) // Modification Error
    {

        ModifyBit(bitstring);

        if (!strcmp(currentmsg_bits.to_string().c_str(), "1000")) // Modification only
        {

            nodemsg->setM_Payload(bitstring.c_str());
            DelayValue = par("PT").doubleValue() + par("TD").doubleValue();
        }

        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1001")) // Delay + modification  Error
        {

            nodemsg->setM_Payload(bitstring.c_str());
            DelayValue = par("PT").doubleValue() + par("TD").doubleValue() + par("ED").doubleValue();
        }
        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1010")) // Modification and Duplication
        {

            nodemsg->setM_Payload(bitstring.c_str());

            nodemsg2->setM_Payload(nodemsg->getM_Payload());
            nodemsg2->setTrailer(nodemsg->getTrailer());
            DelayValue = par("PT").doubleValue() + par("TD").doubleValue();
            dupDelay = par("PT").doubleValue() + par("TD").doubleValue() + par("DD").doubleValue();
        }
        else if (!strcmp(currentmsg_bits.to_string().c_str(), "1011")) // Modification and Duplication +delay
        {

            nodemsg->setM_Payload(bitstring.c_str());

            nodemsg2->setM_Payload(nodemsg->getM_Payload());
            nodemsg2->setTrailer(nodemsg->getTrailer());
            DelayValue = par("PT").doubleValue() + par("TD").doubleValue() + par("ED").doubleValue();
            dupDelay = par("PT").doubleValue() + par("TD").doubleValue() + par("ED").doubleValue() + par("DD").doubleValue();
        }
        else if ((!strcmp(currentmsg_bits.to_string().c_str(), "1100")) || (!strcmp(currentmsg_bits.to_string().c_str(), "1101"))) // Modification and loss || modification + loss + delay
        {
            EV << simTime() << endl;
            nodemsg->setM_Payload(bitstring.c_str());
            scheduleAt(simTime() + par("PT").doubleValue(), new cMessage("lost"));
        }
        else if ((!strcmp(currentmsg_bits.to_string().c_str(), "1110")) || (!strcmp(currentmsg_bits.to_string().c_str(), "1111"))) // Modification and loss  +duplication || modification + loss + duplication + delay
        {

            nodemsg->setM_Payload(bitstring.c_str());
            scheduleAt(simTime() + par("PT").doubleValue(), new cMessage("lost"));
        }
    }
}
void Node::readInputFile(vector<MessageData> &msgs, string filename)
{
    EV << filename << endl;
    string line;

    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        EV << "Error in opening input file in node" << endl;
        return;
    }
    while (getline(inputFile, line))
    {
        EV << line << endl;
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
    if (msg->getArrivalGateId() == 0) /// msg from coordinator
    {

        if (!strcmp(this->getName(), "node0"))
        {
            filename = "input0.txt";
        }
        else if (!strcmp(this->getName(), "node1"))
        {
            filename = "input1.txt";
        }
        readInputFile(msgs, filename);
        int counter = 0;
        while (counter < window_size && !msgs.empty())
        {
            NodeMessage_Base *frame = new NodeMessage_Base("");
            MessageData currentMsg = msgs[counter];
            msgs.erase(msgs.begin());
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
            frame->setM_Payload(bitstring.c_str());
            counter++;
            sendDelayed(frame, par("PT").doubleValue() + par("TD").doubleValue(), "node$o"); // Send frame
        }
    }
    else
    {
        NodeMessage_Base *ReceivedMessage = check_and_cast<NodeMessage_Base *>(msg);
        if (ReceivedMessage->getFrame_Type() == 2) // recieving data [reciever]
        {
            int seqnum = ReceivedMessage->getHeader();
            string payload = ReceivedMessage->getM_Payload(); // Extract payload
            string trailer = ReceivedMessage->getTrailer();   // Extract trailer

            // if (ReceivedMessage->isSelfMessage())
            // {
            //     EV << "At time[" << simTime() << "], Node[" << ReceivedMessage->getHeader() << "] Sending [";
            //     EV << ReceivedMessage->getFrame_Type() << "] with number [" << ReceivedMessage->getACK();
            //     string ack_loss = "YES";
            //     if (uniform(0, 1) > par("LP").doubleValue())
            //     {
            //         ack_loss = "NO";
            //         sendDelayed(ReceivedMessage, par("TD").doubleValue(), "out");
            //     }
            //     EV << "] , loss [" << ack_loss << "]";
            //     return;
            // }
            string str = "";
            std::bitset<8> parity_check = CalculateParityRec(payload, str, bitset<8>(trailer));
            NodeMessage_Base *ack_nack = new NodeMessage_Base("msg");
            if (!strcmp(parity_check.to_string().c_str(), trailer.c_str()))
            {
                // There is an error detected so send NACK
                ack_nack->setHeader(seqnum); // NACK for the received sequence number
                ack_nack->setM_Payload("");  // No payload needed for NACK
                ack_nack->setFrame_Type(0);  // Type for NACK can be set to 0
                ack_nack->setACK(ReceivedMessage->getHeader());
            }
            else if (parity_check == 00000000) // correct frame sent in order
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
        else if (ReceivedMessage->getFrame_Type() == 1) // recieving Ack [sender]
        {
            int ack_num = ReceivedMessage->getACK(); // Get ACK number
            EV << "Received ACK for frame " << ack_num << endl;

            // Check if the ACK is for a frame within the current window
            if (between(frame_expected, ack_num, (frame_expected + window_size) % MAX_SEQ))
            {
                EV << "ACK is within the window." << endl;

                // Move the window forward
                while (frame_expected != (ack_num + 1) % MAX_SEQ)
                {
                    // Free the frame buffer for the acknowledged frame
                    delete send_window[frame_expected % window_size];
                    send_window[frame_expected % window_size] = nullptr;

                    // Update the expected frame
                    frame_expected = (frame_expected + 1) % MAX_SEQ;
                }

                // Send new frames if the window allows
                while (send_window[frame_expected % window_size] == nullptr && !msgs.empty())
                {
                    NodeMessage_Base *frame = new NodeMessage_Base("");
                    MessageData currentMsg = msgs.front(); // Get the next message
                    msgs.erase(msgs.begin());              // Remove the message from the list

                    int seq_num = (frame_expected + window_size - 1) % MAX_SEQ; // Determine the sequence number
                    frame->setHeader(seq_num);                                  // Set the sequence number
                    frame->setFrame_Type(2);                                    // Set as data frame

                    string currentmsg_data = currentMsg.data;
                    bitset<4> errorBits = currentMsg.prefix;

                    // Framing
                    string currentmsg_framed = FrameAndFlag(currentmsg_data);

                    // Parity Calculation
                    string bitstring = "";
                    bitset<8> parity = CalculateParity(currentmsg_framed, bitstring);
                    frame->setTrailer(parity.to_string().c_str());

                    // Setting payload
                    frame->setM_Payload(bitstring.c_str());

                    // Store the frame in the send window
                    send_window[frame_expected % window_size] = frame;

                    // Send the frame
                    sendDelayed(frame->dup(), par("PT").doubleValue() + par("TD").doubleValue(), "node$o");
                }
            }
            else
            {
                EV << "ACK is outside the window, ignoring." << endl;
            }
        }

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
