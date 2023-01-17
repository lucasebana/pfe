
# https://github.com/ashish-gehani/SPADE/wiki/Reporting-provenance-using-JSON

import json
import networkx as nx
from random import randrange



def create_provenance_graph(path = "wsp-audit.log"):
    data = []
    with open(path) as f:
        for line in f:
          data.append(json.loads(line))



    G = nx.MultiDiGraph()
    Nodes = []
    Edges = []


    for i in range(len(data)):
        if data[i]["type"] in ["Entity","Activity","Agent"]:
              Nodes.append(data[i])
        elif data[i]["type"] in ["ActedOnBehalfOf","WasInformedBy","WasDerivedFrom","WasAssociatedWith","WasAttributedTo","Used","WasGeneratedBy"]:
              Edges.append(data[i])


    node_types = ["unknown","string","task","inode_unknown","link","file","directory","char","block","pipe","socket","argv","envp","process_memory","msg","shm","address","sb","path","disc_entity","disc_activity","disc_agent","machine","packet","iattr","xattr","packet_content"]



    ObjectIds = []
    for i in range(len(Nodes)):
        P = Nodes[i]["annotations"].get("object_id",None)
        if(P not in ObjectIds and P != None):
            ObjectIds.append(Nodes[i]["annotations"]["object_id"])
        

    IndexByType = [[] for k in range(len(node_types))]
    for i in range(len(Nodes)):
        J = Nodes[i]["annotations"].get("object_id",None);
        if J != None: 
            #print(Nodes[i]["annotations"]["object_type"])
            index = node_types.index(Nodes[i]["annotations"]["object_type"])
            if( J not in IndexByType[index]):
                IndexByType[index].append(J)
        
    IndexById = []

    for i in range(len(Nodes)):
        J = Nodes[i]["annotations"].get("object_id",None);
        index = node_types.index(Nodes[i]["annotations"]["object_type"])
        if J != None:
            IndexById.append(IndexByType[index].index(J))
        else:
            IndexById.append(-1)

    Senders = []
    Receivers = []
    for i in range(len(Nodes)):
        NiA = Nodes[i]["annotations"];
        if(NiA["object_type"] == "packet"):
            if NiA["sender"] not in Senders:
                Senders.append(NiA["sender"])
            if NiA["receiver"] not in Receivers:
                Receivers.append(NiA["receiver"])
        


    SendersByI = []
    ReceiversByI = []
    MapIP = dict()
    for i in range(len(Nodes)):
        NiA = Nodes[i]["annotations"];
        if(NiA["object_type"] == "packet"):
            SendersByI.append(chr(65 + Senders.index(Nodes[i]["annotations"]["sender"])))
            MapIP[chr(65 + Senders.index(Nodes[i]["annotations"]["sender"]))] = Nodes[i]["annotations"]["sender"]
            if Nodes[i]["annotations"]["receiver"] in Senders:
                ReceiversByI.append(chr(65 + Senders.index(Nodes[i]["annotations"]["receiver"])))
                MapIP[chr(65 + Senders.index(Nodes[i]["annotations"]["receiver"]))] = Nodes[i]["annotations"]["receiver"]
            else :
                ReceiversByI.append(chr(65 + len(Senders) + Senders.index(Nodes[i]["annotations"]["receiver"])))
                MapIP[chr(65 + len(Senders) + Senders.index(Nodes[i]["annotations"]["receiver"]))] = Nodes[i]["annotations"]["receiver"]
        else:
            SendersByI.append("")
            ReceiversByI.append("")


    for k in MapIP:
        G.add_node("IP" + k, label = k + "=" +MapIP[k])

    for i in range(len(Nodes)):
        N = Nodes
        NiA = Nodes[i]["annotations"]

        labeli = ""
        pid = NiA.get("pid","")
        if pid != "":
            pid = " " + str(pid) + "";

        value = NiA.get("value","")
        

        ips = ""

        if(NiA["object_type"] == "packet"):
            ips = SendersByI[i] + " ->" + ReceiversByI[i]
            pass
            
        if(NiA["object_type"] == "process_memory"):
            #print("Process : ",    Nodes[i])
            pass
        if(NiA["object_type"] == "task"):
            #print("Task : ",    Nodes[i])
            pass
        if(NiA["object_type"] == "argv"):
            #print("Argv : ",    Nodes[i])
            pass

        if(NiA["object_type"] == "socket"):
            #print("Socket : ",    Nodes[i])
            pass

        
        if(NiA["object_type"] == "argv"):
            value = " " + str(NiA.get("value","")) + "";

        if NiA.get("pathname",None) != None:
            labeli = NiA["object_type"] + " : " + NiA.get("pathname");
        else:
            labeli = NiA["object_type"] + "{" + str(IndexById[i])+  "}" + pid + value + ips


        G.add_node(N[i]["id"],
        label=labeli,
        group=node_types.index(NiA["object_type"]),
        type=N[i]["type"],
        objectid=NiA.get("object_id",""),
        objecttype=NiA["object_type"],
        data=NiA,
        pathname=NiA.get("pathname","None"),
        shape="box")
        
        '''
        G.add_node(N[i]["id"],
        label=NiA["object_type"],
        group=ObjectIds.index(NiA["object_id"]),
        type=N[i]["type"],
        objectid=NiA["object_id"],
        objecttype=NiA["object_type"],
        shape="box")
        '''




    for i in range(len(Edges)):
        G.add_edge(Edges[i]["from"],Edges[i]["to"],annotations=Edges[i]["annotations"],label=Edges[i]["annotations"]["relation_type"])
    return G

#create_provenance_graph("audit/fork.audit.log")
g = create_provenance_graph("attaque/audit.log")