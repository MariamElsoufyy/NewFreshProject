package projectgdeed.simulations;

import ned.IdealChannel;
import projectgdeed.Coordinator;
import projectgdeed.Node;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    submodules:
        node0: Node {
            @display("p=53,64");
        }
        node1: Node {
            @display("p=131,92");
        }
        coordinator: Coordinator {
            @display("p=99,151");
        }
    connections:
        coordinator.ports++ <--> IdealChannel <--> node0.coordinator;
        coordinator.ports++ <--> IdealChannel <--> node1.coordinator;
        node0.node <--> IdealChannel <--> node1.node;
}

