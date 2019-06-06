namespace ACM_INTERNAL{

	void MODULE_MANAGER::createSockets()
	{
		if (SocketsInitialized)
		{
			LOGGER(ERROR,"Sockets already initialized");
			return;
		}

		try{
			if(PARENT_NAME.size()!=0 || childrenModules.size()!=0)
			{
				Send_Socket=new UDPSocket();
				if(this->isSystemModuleManager)
				{
					Recv_Socket = new UDPSocket(this->PARENT_PORT);
				}
				else
				{

					if (this->isMulticast)
					{
						//Only Children Join Multicast Group
						Recv_Socket = new UDPSocket();
						Recv_Socket->setReuse();
						Recv_Socket->clearBuffer();
						Recv_Socket->setLocalPort(CONFIGURATION_PORT_NUM);
						Recv_Socket->joinGroup(this->broadcastaddress);
					}
					else
					{
						Recv_Socket = new UDPSocket(CONFIGURATION_PORT_NUM);

					}
				}

				if (Send_Socket==0)
				{
					LOGGER(ERROR,"creating sendSocket");
					return;
				}
				if (Recv_Socket==0)
				{
					LOGGER(ERROR,"creating Recv_Socket");
					return;
				}
			}
		}
		catch(SocketException& e)
		{
			LOGGER(FATAL,"%s",e.what());
			throw(-3);
		}
	}

	void MODULE_MANAGER::PREPARE_PARENT_CHILD_INFORMATION()
	{
		//RETURN_CODE_TYPE RETURN_CODE;
		std::string SEARCH_STRING=PRIMARY_KEY_SYNCHRONIZED_EXECUTION;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair = this->PropertyMap.equal_range(SEARCH_STRING);
		if(searchPair.first==searchPair.second)
		{
			LOGGER(INFO,"%s is not set",PRIMARY_KEY_SYNCHRONIZED_EXECUTION);
			return;
		}
		else
		{
			std::string value=searchPair.first->second;
			std::transform(value.begin(), value.end(), value.begin(),  (int(*)(int)) toupper);
			if (value.compare("TRUE")!=0)
			{
				LOGGER(INFO,"%s is  set to %s. Not running in synchronized mode",PRIMARY_KEY_SYNCHRONIZED_EXECUTION,(searchPair.first->second).c_str());
				return;			
			}
		}

		LOGGER(INFO,"%s is  set to %s. running in synchronized mode",PRIMARY_KEY_SYNCHRONIZED_EXECUTION,(searchPair.first->second).c_str());		
		/////////////////////////////////////////////////////	

		//Get the broad cast address PRIMARY_KEY_BROADCAST_ADDRESS
		SEARCH_STRING=PRIMARY_KEY_BROADCAST_ADDRESS;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);

		if(searchPair.first==searchPair.second)
		{
			LOGGER(ERROR,"No broadcast address ");
			this->isBroadcast=false;
		}
		else
		{
			this->broadcastaddress=searchPair.first->second;
			this->isBroadcast=true;
		}

		//Get the Multi cast address PRIMARY_KEY_BROADCAST_ADDRESS
		SEARCH_STRING=PRIMARY_KEY_MULTICAST_ADDRESS;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);

		if(searchPair.first==searchPair.second)
		{
			LOGGER(ERROR,"No multicast address ");
			this->isMulticast=false;
		}
		else
		{
			this->broadcastaddress=searchPair.first->second;
			this->isMulticast=true;
		}

		if(this->isMulticast && this->isBroadcast)
		{
			LOGGER(FATAL,"Cannot specify both %s and %s",PRIMARY_KEY_MULTICAST_ADDRESS,PRIMARY_KEY_BROADCAST_ADDRESS);
			throw(-3);
		}

		if(! (this->isMulticast) && ! (this->isBroadcast))
		{
			LOGGER(FATAL,"Should specify either %s or %s. Cannot Proceed in synchronized mode",PRIMARY_KEY_MULTICAST_ADDRESS,PRIMARY_KEY_BROADCAST_ADDRESS);
			throw(-3);
		}



		SEARCH_STRING=PRIMARY_KEY_CONFIGURATION_PORT;

		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);

		if(searchPair.first!=searchPair.second)
		{

			//Found configuration port
			std::string tempvalue=searchPair.first->second;
			if(!from_string<unsigned int>(this->CONFIGURATION_PORT_NUM,tempvalue,std::dec) || this->CONFIGURATION_PORT_NUM==0)
			{
				LOGGER(FATAL,"Incorrect%s %s.",PRIMARY_KEY_CONFIGURATION_PORT,tempvalue.c_str());
				throw(-2);
			}			
		}
		else
		{
			LOGGER(ERROR,"CONFIGURATION_PORT_NUM Not Defined. Cannot Use for synchronized start up");
			throw(-2);
		}


		//Searching for Parent Port

		SEARCH_STRING=PRIMARY_KEY_PARENT_PORT;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);

		if(searchPair.first!=searchPair.second)
		{

			//Found configuration port
			std::string tempvalue=searchPair.first->second;
			if(!from_string<unsigned int>(this->PARENT_PORT,tempvalue,std::dec) || this->PARENT_PORT==0)
			{
				LOGGER(FATAL,"Incorrect%s %s.",PRIMARY_KEY_PARENT_PORT,tempvalue.c_str());
				throw(-2);
			}
		}
		else
		{
			LOGGER(ERROR,"PARENT_PORT Not Defined. Cannot Use for synchronized start up");
			throw(-2);
		}



		SEARCH_STRING=PRIMARY_KEY_MODULE_NAME;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		if(searchPair.first==searchPair.second)
		{
			LOGGER(ERROR,"MODULE_NAME Not Defined. Cannot Use for synchronized start up");
			throw(-2);
		}
		else
		{
			this->MODULE_NAME=searchPair.first->second;			
		}

		SEARCH_STRING=PRIMARY_KEY_PARENT_MODULE_NAME;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		if(searchPair.first==searchPair.second)
		{
			LOGGER(INFO,"Parent Module Not Set. This must be the system module manager");
			this->isSystemModuleManager=true;

			SEARCH_STRING=PRIMARY_KEY_CHILD_MODULE_NAME;
			searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
			for(PropertyMapIteratorType iter =searchPair.first; iter !=searchPair.second;iter++)
			{
				child_url temp;
				temp.childname=iter->second;
				temp.portnum=this->CONFIGURATION_PORT_NUM;
				std::string SEARCH_STRING_CHILD=temp.childname+ADDITVE_KEY_MM_HOSTNAME;
				std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPairHostname;
				searchPairHostname=this->PropertyMap.equal_range(SEARCH_STRING_CHILD);
				if(searchPairHostname.first==searchPairHostname.second)
				{
					LOGGER(ERROR,"Could Not find entry for %s",SEARCH_STRING_CHILD.c_str());
					throw(-3);
				}
				else
				{
					temp.hostname=searchPairHostname.first->second;
				}

				this->childrenModules[temp.childname]=temp;
				// do something
			}
		}
		else
		{
			LOGGER(INFO,"Parent Module  Set. This is not the system module manager. It cannot have children");
			this->isSystemModuleManager=false;
			this->isrunningUnderSystemModuleManager=true;

			this->PARENT_NAME=searchPair.first->second;
			//	this->PARENT_PORT=this->CONFIGURATION_PORT_NUM;
			SEARCH_STRING=this->PARENT_NAME+ADDITVE_KEY_MM_HOSTNAME;
			searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
			if(searchPair.first==searchPair.second)
			{
				LOGGER(ERROR,"Could Not find entry for %s",SEARCH_STRING.c_str());
				throw(-3);
			}
			this->PARENT_HOST=searchPair.first->second;

			SEARCH_STRING=PRIMARY_KEY_CHILD_MODULE_NAME;
			searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
			if(searchPair.first!=searchPair.second)
			{
				LOGGER(ERROR,"Invalid COnfiguration. Both parent and Child Are set. Ignoring Children Modules");
				throw(-3);
			}
		}		
	}


	bool MODULE_MANAGER::GET_READY_MM_FOR_HYPERPERIOD()
	{
		if(!this->isSystemModuleManager && !this->isrunningUnderSystemModuleManager)
			return true;

		if(this->isSystemModuleManager)
		{
			this->resetchildrenModules();

			ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage;

			while(this->numchildrensyncremaining()>0)
			{
				unsigned int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				LOGGER(INFO,"Trying to Receive BARRIER_READY from Child ");
				recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress, sourcePort);
				if(recvMsgSize!=SyncTypeSize) continue;
				else
				{
					ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(SocketSynchronizationMessage,BUFFER);
					if(SocketSynchronizationMessage.Sync!=SocketSynchronizationMessageType::BARRIER_READY) continue;

					std::string src=SocketSynchronizationMessage.MESSAGE_SRC_MODULE;
					this->childrenModules[src].receivedanswer=true;
				}
			}


		}
		else
		{
			//send READY to Parent
			ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage(ACM_INTERNAL::SocketSynchronizationMessageType::BARRIER_READY,this->MODULE_NAME,this->PARENT_NAME);
			ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSynchronizationMessage);
			this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,this->PARENT_HOST,this->PARENT_PORT);						 
		}	

		return true;

	}


	void MODULE_MANAGER::DETERMINE_AVERAGE_NETWORK_LATENCY()
	{
		if(!this->isSystemModuleManager && !this->isrunningUnderSystemModuleManager)
			return;

		if(this->isSystemModuleManager)
		{
			//For Each Child Send a Ready
			//Child Sends ACK
			//Parent Sends Ack
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSyncMessage(ACM_INTERNAL::SocketSynchronizationMessageType::NETWORK_LATENCY_READY,this->MODULE_NAME,this->PARENT_NAME);	
			ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
			ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSyncMessage);

			for (std::map<std::string,child_url>::iterator iter=childrenModules.begin();iter!=childrenModules.end();iter++)
			{
				SocketSyncMessage.Sync=SocketSynchronizationMessageType::NETWORK_LATENCY_READY;
				ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSyncMessage);
				this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,iter->second.hostname,iter->second.portnum);

				//Wait for Ready
				ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage;
				LOGGER(INFO,"DETERMINE_AVERAGE_NETWORK_LATENCY: Sent NETWORK_LATENCY_READY to %s. Wait for NETWORK_LATENCY_CHILD_ACK.",iter->second.childname.c_str());
				int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				bool receivedProper=false;
				while(!receivedProper)
				{
					recvMsgSize=0;
					while(recvMsgSize!=SyncTypeSize)
					{
						recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress, sourcePort);
						if(recvMsgSize!=SyncTypeSize) 
						{
							LOGGER(ERROR,"DETERMINE_AVERAGE_NETWORK_LATENCY: Bad NETWORK_LATENCY_READY from %s.",iter->second.childname.c_str());						
						}
					}

					ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(SocketSynchronizationMessage,BUFFER);
					if(SocketSynchronizationMessage.Sync==SocketSynchronizationMessageType::NETWORK_LATENCY_CHILD_ACK) 
						receivedProper=true;
				}


				SocketSynchronizationMessage.printout();

				if(SocketSynchronizationMessage.Sync==SocketSynchronizationMessageType::NETWORK_LATENCY_CHILD_ACK) 
				{

					LOGGER(INFO,"DETERMINE_AVERAGE_NETWORK_LATENCY: Received NETWORK_LATENCY_CHILD_ACK ");
					LOGGER(INFO,"DETERMINE_AVERAGE_NETWORK_LATENCY: Sending NEWORK_LATENCY_PARENT_ACK ");


					SocketSyncMessage.Sync=SocketSynchronizationMessageType::NEWORK_LATENCY_PARENT_ACK;
					ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSyncMessage);
					this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,iter->second.hostname,iter->second.portnum);					
				}				
			}


			//Send a broad Cast go




			SocketSyncMessage.Sync=ACM_INTERNAL::SocketSynchronizationMessageType::START;
			ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSyncMessage);
			LOGGER(INFO,"SENDING Go After Latency");
			this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,this->broadcastaddress,this->CONFIGURATION_PORT_NUM);					
		}
		else
		{
			ACM_INTERNAL::SocketSynchronizationMessageType syncrecv;
			//Wait to Receive ACK from parent
			ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
			while(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0 )
			{
				int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				LOGGER(INFO,"Trying to Receive NETWORK_LATENCY_READY from Parent ");
				recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress,	sourcePort);
				LOGGER(INFO,"Received Message Size %d",recvMsgSize);
				if (recvMsgSize!=SyncTypeSize || errno==EAGAIN || errno==EWOULDBLOCK)
				{
					LOGGER(ERROR,"Error in Receiving NETWORK_LATENCY_READY from Parent");					
				}
				ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(syncrecv,BUFFER);

				if(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0)
				{
					LOGGER(ERROR,"RECEIVED WRONG ENTRY %s:%s",this->PARENT_NAME.c_str(),syncrecv.MESSAGE_SRC_MODULE);
					syncrecv.printout();
				}
			}

			if (syncrecv.Sync!=SocketSynchronizationMessageType::NETWORK_LATENCY_READY) 
			{
				LOGGER(ERROR,"RECEIVED WRONG synchronization message type. expecting NETWORK_LATENCY_READY");
				syncrecv.printout();
				return;
			}

			SYSTEM_TIME_TYPE current= currentTimeInNanoSeconds();
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSyncMessage(ACM_INTERNAL::SocketSynchronizationMessageType::NETWORK_LATENCY_CHILD_ACK,this->MODULE_NAME,this->PARENT_NAME);	


			//send it to parent

			ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSyncMessage);
			this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,this->PARENT_HOST,this->PARENT_PORT);		

			SYSTEM_TIME_TYPE reciept=current;

			strcpy(syncrecv.MESSAGE_SRC_MODULE," "); //resettting it

			while(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0 )
			{
				int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				LOGGER(INFO,"Trying to Receive NEWORK_LATENCY_PARENT_ACK from Parent ");
				recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress,	sourcePort);
				reciept= currentTimeInNanoSeconds();
				LOGGER(INFO,"Received Message Size %d",recvMsgSize);
				if (recvMsgSize!=SyncTypeSize || errno==EAGAIN || errno==EWOULDBLOCK)
				{
					LOGGER(ERROR,"Error in Receiving NEWORK_LATENCY_PARENT_ACK from Parent");					
				}
				ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(syncrecv,BUFFER);

				if(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0)
				{
					LOGGER(ERROR,"RECEIVED WRONG ENTRY for the parent name");
					syncrecv.printout();
				}
			}

			if (syncrecv.Sync!=SocketSynchronizationMessageType::NEWORK_LATENCY_PARENT_ACK) 
			{
				LOGGER(ERROR,"RECEIVED WRONG ENTRY. expected NEWORK_LATENCY_PARENT_ACK ");
				syncrecv.printout();
				return;
			}
			else
			{
				this->AVE_NETWORK_LATENCY_FROM_PARENT=reciept-current;
				LOGGER(INFO,"Network Latency in NS %lld",this->AVE_NETWORK_LATENCY_FROM_PARENT);
			}



			//Child Wait for hyper period
			//	strcpy(syncrecv.MESSAGE_SRC_MODULE," "); //resettting it
			//Wait to Receive Start from parent

			while(true)
			{
				strcpy(syncrecv.MESSAGE_SRC_MODULE," ");
				while(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0 )
				{
					unsigned int recvMsgSize=0;
					string sourceAddress;             // Address of datagram source
					unsigned short sourcePort;        // Port of datagram source
					LOGGER(INFO,"Trying to Receive START from Parent ");
					recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress,	sourcePort);
					LOGGER(INFO,"Received Message Size %u",recvMsgSize);
					if (recvMsgSize!=SyncTypeSize || errno==EAGAIN || errno==EWOULDBLOCK)
					{
						LOGGER(ERROR,"Error in Receiving START from Parent");
						continue;
					}

					ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(syncrecv,BUFFER);
					if(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0)
					{
						LOGGER(ERROR,"RECEIVED WRONG ENTRY");
						syncrecv.printout();
					}
				}

				if (syncrecv.Sync==SocketSynchronizationMessageType::START) break;
			}		


		}



		LOGGER(INFO,"Out of determine Network latency");

	}
	bool MODULE_MANAGER::SYNC_WITH_OTHER_MODULE_MANAGERS()
	{

		if(!this->isSystemModuleManager && !this->isrunningUnderSystemModuleManager)
			return true;

		//1. Children send hi to parent. Parent waits for hi
		//2. Parent Broadcasts go. Children waits for go.
		if(this->isSystemModuleManager)
		{
			this->resetchildrenModules();

			ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage;
			LOGGER(INFO,"isSystemModuleManager. syncing with others");

			while(this->numchildrensyncremaining()>0)
			{
				unsigned int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				LOGGER(INFO,"Trying to Receive ready from children ");
				recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress, sourcePort);
				if(recvMsgSize!=SyncTypeSize) continue;
				else
				{
					LOGGER(INFO,"Received Message from CHild");

					ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(SocketSynchronizationMessage,BUFFER);
					SocketSynchronizationMessage.printout();
					if(SocketSynchronizationMessage.Sync!=SocketSynchronizationMessageType::BARRIER_READY) continue;

					std::string src=SocketSynchronizationMessage.MESSAGE_SRC_MODULE;
					this->childrenModules[src].receivedanswer=true;
				}
			}

			//Broadcase
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSyncMessage(ACM_INTERNAL::SocketSynchronizationMessageType::BARRIER_ACK,this->MODULE_NAME,this->PARENT_NAME);
			ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSyncMessage);
			LOGGER(INFO,"isSystemModuleManager.Sending BARRIER_ACK");
			this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,this->broadcastaddress,this->CONFIGURATION_PORT_NUM);
			//	this->Recv_Socket->clearBuffer();
			return true;

		}
		else
		{
			LOGGER(INFO,"isnotSystemModuleManager. syncing with others");
			//send READY to Parent
			ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
			ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage(ACM_INTERNAL::SocketSynchronizationMessageType::BARRIER_READY,this->MODULE_NAME,this->PARENT_NAME);
			ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSynchronizationMessage);
			this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,this->PARENT_HOST,this->PARENT_PORT);

			ACM_INTERNAL::SocketSynchronizationMessageType syncrecv;
			//Wait to Receive ACK from parent

			while(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0 )
			{
				unsigned int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				LOGGER(INFO,"Trying to Receive ACK from Parent ");
				recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress,	sourcePort);
				LOGGER(INFO,"Received Message Size %u",recvMsgSize);
				if (recvMsgSize!=SyncTypeSize || errno==EAGAIN || errno==EWOULDBLOCK)
				{
					LOGGER(ERROR,"Error in Receiving ACK from Parent");
					return false;
				}
				ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(syncrecv,BUFFER);
				if(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0)
				{
					LOGGER(ERROR,"RECEIVED WRONG ENTRY");
					syncrecv.printout();
				}
			}

			if (syncrecv.Sync==SocketSynchronizationMessageType::BARRIER_ACK)
				return true;

			else
			{
				syncrecv.printout();
				return false;
			}
		}

	}


	void MODULE_MANAGER::SEND_HYPER_PERIOD_ECHO(bool Stop)
	{
		if(!(this->isSystemModuleManager)) return;
		static ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
		static ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage(ACM_INTERNAL::SocketSynchronizationMessageType::START,this->MODULE_NAME,this->PARENT_NAME);

		if(Stop) SocketSynchronizationMessage.Sync=ACM_INTERNAL::SocketSynchronizationMessageType::STOP;

		ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(BUFFER,SocketSynchronizationMessage);
		LOGGER(INFO,"SENDING HYPERPERIOD START");
		this->Send_Socket->sendTo(&BUFFER,SyncTypeSize,this->broadcastaddress,this->CONFIGURATION_PORT_NUM);
	}

};