package projectgdeed;

import ned.IdealChannel;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    submodules:
        node0: Node {
            @display("p=55,49");
        }
        node1: Node {
            @display("p=130,61");
        }
        coordinator: Coordinator {
            @display("p=85,159");
        }
    connections:
        coordinator.ports++ <--> IdealChannel <--> node0.coordinator;
        coordinator.ports++ <--> IdealChannel <--> node1.coordinator;
        node0.node <--> IdealChannel <--> node1.node;
}

