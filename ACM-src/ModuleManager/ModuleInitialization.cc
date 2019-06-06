namespace ACM_INTERNAL{
	void MODULE_MANAGER::INIT_PARTITION_PROPERTY_MAP(std::string filename)
	{		
		std::ifstream myfile (filename.c_str());
		if(!myfile)
		{
			LOGGER(ACM_USER::FATAL,"Cannot open file %s",filename.c_str());
			throw (-1);
		}
		std::string line;
		while (! myfile.eof() )
		{
			std::getline (myfile,line);	

			//Check if line is empty
			stringTrim(line," \t\r\n\0");
			if (line.size()==0) {
				//LOGGER(INFO,"EMPTY LINE");
				continue;
			}


			//check if its a comment

			std::string::size_type lastPos = line.find_first_of("//", 0);
			if (lastPos==0)
				continue;

			std::vector <std::string> tokens;
			tokens.clear();
			stringTokenize(line,tokens,"=");		
			if (tokens.size()!=2)
			{
				LOGGER(ACM_USER::FATAL,"Wrong tokens for line %s",line.c_str());
				myfile.close();
				throw (-1);
			}
			//std::string value =tokens[1];

			//	printf("%s,%s,size%d,size%d\n",tokens[0].c_str(),value.c_str(),tokens[0].length(),value.length());
			this->PropertyMap.insert(std::pair<std::string,std::string>(tokens[0],tokens[1]));	
		}

		if (this->PropertyMap.count(std::string(PRIMARY_KEY_HYPERPERIOD_KEY))!=1)
		{
			LOGGER(ACM_USER::FATAL,"HYPERPERIOD Value not given in the property file");
		}

		for (std::map<std::string,std::string>::iterator iter=PropertyMap.begin();iter!=PropertyMap.end();iter++)
		{
			LOGGER(INFO,"%s=%s",iter->first.c_str(),iter->second.c_str());
		}
	}
	void MODULE_MANAGER::READ_NUMBER_OF_INIT_STAGES()
	{
		std::string SEARCH_STRING=PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		char bufstr[100];
		if(searchPair.first==searchPair.second)
		{

			sprintf(bufstr," %s not set. Will  use 1", PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES);
			LOGGER(INFO,bufstr);
			NumberOfInitStages=1;
			return;			
		}

		std::string stages=searchPair.first->second;		
		stringTrim(stages," ");
		if(!from_string<unsigned int>(this->NumberOfInitStages,stages,std::dec) || this->NumberOfInitStages>2)
		{
			LOGGER(FATAL,"Incorrect%s %s. Legal values =1 or 2",PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES,stages.c_str());
			throw(-2);
		}		
	}
	void  MODULE_MANAGER::CREATE_SAMPLING_CHANNELS()
	{
		RETURN_CODE_TYPE RETURN_CODE;
		for (unsigned int counter=0;counter< this->PARTITION_VECTOR.size();counter++)
		{
			for (unsigned int k=0;k<this->PARTITION_VECTOR[counter].SAMPLING_PORTS.size();k++)
			{
				if (this->PARTITION_VECTOR[counter].SAMPLING_PORTS[k].PORT_DIRECTION==ACM_USER::DESTINATION)
					continue;
				CHANNEL_MAP_TYPE::iterator source_iterator;			
				for (CHANNEL_MAP_TYPE::iterator iter=CHANNEL_MAP.begin();iter!=CHANNEL_MAP.end();iter++)
				{
					if (iter->second.source.compare(this->PARTITION_VECTOR[counter].SAMPLING_PORTS[k].Name)==0)
					{
						//SAMPLING_CHANNEL * channel = new SAMPLING_CHANNEL(iter->first); //channel name
						if (CanAddChannel(iter->first))
						{
							int currentsize= this->PARTITION_VECTOR[counter].SAMPLING_CHANNEL_SOURCE_VECTOR.size();
							LOGGER(INFO,"SAMPLING CHANNEL NUMBER %d ", currentsize);
							//this->PARTITION_VECTOR[counter].SAMPLING_CHANNEL_SOURCE_VECTOR.resize(currentsize+1);
							SAMPLING_CHANNEL* temp_channel=new SAMPLING_CHANNEL();

							//this->PARTITION_VECTOR[counter].SAMPLING_CHANNEL_SOURCE_VECTOR.push_back();
							temp_channel->SET_NAME(iter->first);


							temp_channel->ADD_SOURCE(this->PARTITION_VECTOR[counter].SAMPLING_PORTS[k].Name,this->PARTITION_VECTOR[counter].SAMPLING_PORTS[k].MAX_MESSAGE_SIZE,this->PARTITION_VECTOR[counter].SAMPLING_PORTS[k].REFRESH_PERIOD, &RETURN_CODE);
							ENSURE_CONDITION(RETURN_CODE==NO_ERROR || RETURN_CODE==NO_ACTION);
							unsigned int sizevec=0;
							bool foundinproxy=false;
							LOGGER(INFO,"Destiantion size %d",iter->second.destination.size());
							for (unsigned int i=0;i<iter->second.destination.size();i++)
							{
								std::string destination_name= iter->second.destination[i];
								SAMPLING_PORT_DESCRIPTOR descriptor;
								int j= FIND_PARTITION_FROM_SAMPLING_PORT(destination_name,descriptor);
								LOGGER(INFO,"output of FIND_PARTITION_FROM_SAMPLING_PORT %d",j);
								if(j==-1)
								{


									//check if name exists in a proxy_port and the direction of proxy port is destination...

									for(;sizevec<this->PROXY_PORTS.size();sizevec++)
									{
										if (PROXY_PORTS[sizevec].Name.compare(destination_name)==0)
										{
											foundinproxy=true;
											break;
										}
									}						

									if(!foundinproxy)
									{
										LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
										throw(-1);
									}
									else
									{
										if(PROXY_PORTS[sizevec].PORT_DIRECTION!=ACM_USER::DESTINATION)
										{
											LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
											PROXY_PORTS[sizevec].printout();
											throw(-1);

										}
									}
								}
								if(foundinproxy)
								{
									RETURN_CODE_TYPE return_code;
									if (temp_channel->GET_COMMUNICATION_TYPE()!=ACM_INTERNAL::INTER_MODULE_SRC)
									{
										temp_channel->SET_COMMUNICATION_TYPE_ENUM(ACM_INTERNAL::INTER_MODULE_SRC,&RETURN_CODE);
										ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
									}
									temp_channel->ADD_DESTINATION_URL(PROXY_PORTS[sizevec].hostName,PROXY_PORTS[sizevec].portNumber,&return_code);
									ENSURE_CONDITION(RETURN_CODE==NO_ERROR);								
								}
								else if(j>=0)
								{
									if(descriptor.PORT_DIRECTION!=ACM_USER::DESTINATION)
									{
										LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
										descriptor.printout();
										throw(-1);
									}
									descriptor.printout();
									temp_channel->ADD_DESTINATION(descriptor.Name,descriptor.MAX_MESSAGE_SIZE,descriptor.REFRESH_PERIOD, &RETURN_CODE);
									ENSURE_CONDITION(RETURN_CODE==NO_ERROR || RETURN_CODE==NO_ACTION);
								}




							}
							temp_channel->SET_INITIALIZED();
							this->PARTITION_VECTOR[counter].SAMPLING_CHANNEL_SOURCE_VECTOR.push_back(temp_channel->GET_NAME());
							this->MAP_OF_SAMPLING_CHANNELS[temp_channel->GET_NAME()]=temp_channel;

							//create the channel
						}
						break;
					}
				}						
			}	

		}

		//////////////////////////////



		//////////////// Inter Module Destination
		unsigned int sizevec=0;
		for(;sizevec<this->PROXY_PORTS.size();sizevec++)
		{
			if(PROXY_PORTS[sizevec].PORT_DIRECTION==ACM_USER::DESTINATION || PROXY_PORTS[sizevec].PROXY_TYPE==PROXY_PORT_DESCRIPTORS::QUEUEING ) continue;

			//this port is a source port
			CHANNEL_MAP_TYPE::iterator source_iterator;
			for (CHANNEL_MAP_TYPE::iterator iter=CHANNEL_MAP.begin();iter!=CHANNEL_MAP.end();iter++)
			{
				if (iter->second.source.compare(this->PROXY_PORTS[sizevec].Name)==0)
				{
					if (CanAddChannel(iter->first))
					{

						SAMPLING_CHANNEL * channel = new SAMPLING_CHANNEL(std::string(iter->first),INTER_MODULE_DST,PROXY_PORTS[sizevec].portNumber); //channel name
						channel->SET_MAX_SIZE_OF_MESSAGE(PROXY_PORTS[sizevec].MAX_MESSAGE_SIZE,&RETURN_CODE);
						channel->SET_MAX_SOCKET_MESSAGE(PROXY_PORTS[sizevec].MAX_NB_MESSAGE,&RETURN_CODE);
						ENSURE_CONDITION(RETURN_CODE==NO_ERROR);

						for (unsigned int i=0;i<iter->second.destination.size();i++)
						{
							std::string destination_name= iter->second.destination[i];
							SAMPLING_PORT_DESCRIPTOR descriptor;
							//unsigned int sizevec=0;

							int j= FIND_PARTITION_FROM_SAMPLING_PORT(destination_name,descriptor);
							if(j==-1) //check if name exists in a proxy_port
							{
								//check if name exists in a proxy_port and the direction of proxy port is destination...						
								LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
								throw(-1);

							}
							if(j>=0)
							{
								if(descriptor.PORT_DIRECTION!=ACM_USER::DESTINATION)
								{
									LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
									descriptor.printout();
									throw(-1);
								}
								channel->ADD_DESTINATION(descriptor.Name,descriptor.MAX_MESSAGE_SIZE,descriptor.REFRESH_PERIOD, &RETURN_CODE);

								bool alreadyInpartition=false;
								for(unsigned int counter=0;counter<this->PARTITION_VECTOR[j].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.size();counter++)
								{
									if(this->PARTITION_VECTOR[j].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[counter].compare(channel->GET_NAME())==0)
									{
										alreadyInpartition=true;
										break;
									}

								}
								if(!alreadyInpartition)
									this->PARTITION_VECTOR[j].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.push_back(channel->GET_NAME());

								ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
							}							
						}						
						//create the channel
						channel->SET_INITIALIZED();					



						this->MAP_OF_SAMPLING_CHANNELS[channel->GET_NAME()]=channel;

						//this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR.push_back(channel);
						//this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR[currentsize].SET_INITIALIZED();
					}
					break;
				}
			}
		}
	}
	bool MODULE_MANAGER::CanAddChannel(std::string name)
	{
		LOGGER(INFO,"Checking if I can add Channel %s",name.c_str());
#ifdef ISNETWORK_PARTITION
		for (unsigned int counter=0;counter< this->PARTITION_VECTOR.size();counter++)
		{
			if (!(this->PARTITION_VECTOR[counter].isNetworkPartition)) continue;

			if (net_partition==0)
			{
				LOGGER(ERROR,"Net partition is 0");
				LOGGER(INFO,"Cannot add Channel %s",name.c_str());
				return false;
			}

			if (net_partition->name.compare(this->PARTITION_VECTOR[counter].Name)!=0) continue;

			if(this->PARTITION_VECTOR[counter].CheckIfChannelExists(name)) //exists in  a network partition
			{
				LOGGER(INFO,"Can add Channel %s in %s",name.c_str(),this->PARTITION_VECTOR[counter].Name.c_str());
				return true;			
			}
		}

		LOGGER(INFO,"Cannot add Channel %s",name.c_str());
		return false; //does not exist in a network partition
#else
		for (unsigned int counter=0;counter< this->PARTITION_VECTOR.size();counter++)
		{
			LOGGER(INFO,"Checking in Partition %s",this->PARTITION_VECTOR[counter].Name.c_str());
			this->PARTITION_VECTOR[counter].printDescriptor();

			if (!(this->PARTITION_VECTOR[counter].isNetworkPartition)) continue;

			if(this->PARTITION_VECTOR[counter].CheckIfChannelExists(name)) //exists in  a network partition
			{
				LOGGER(INFO,"Cannot add Channel %s",name.c_str());
				return false;			
			}
		}

		LOGGER(INFO,"Sure Can add Channel %s",name.c_str());
		return true; //does not exist in a network partition
#endif
	}
	void MODULE_MANAGER::CREATE_QUEUEING_CHANNELS()
	{

		RETURN_CODE_TYPE RETURN_CODE;

		//for source channels...
		for (unsigned int counter=0;counter< this->PARTITION_VECTOR.size();counter++)
		{
			for (unsigned int k=0;k<this->PARTITION_VECTOR[counter].QUEUING_PORTS.size();k++)
			{
				if (this->PARTITION_VECTOR[counter].QUEUING_PORTS[k].PORT_DIRECTION==ACM_USER::DESTINATION)
					continue;
				CHANNEL_MAP_TYPE::iterator source_iterator;
				for (CHANNEL_MAP_TYPE::iterator iter=CHANNEL_MAP.begin();iter!=CHANNEL_MAP.end();iter++)
				{
					if (iter->second.source.compare(this->PARTITION_VECTOR[counter].QUEUING_PORTS[k].Name)==0)
					{
						if (CanAddChannel(iter->first))
						{
							QUEUE_CHANNEL * channel = new QUEUE_CHANNEL(); //channel name
							int currentsize= this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR.size();
							LOGGER(INFO,"Partition %s, QUEUE CHANNEL NUMBER %d. Channel name %s ", this->PARTITION_VECTOR[counter].Name.c_str(),currentsize,iter->first.c_str());
							//this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR.resize(currentsize+1);
							//this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR.push_back(QUEUE_CHANNEL());
							channel->SET_NAME(iter->first);						
							channel->ADD_SOURCE_QUEUE(this->PARTITION_VECTOR[counter].QUEUING_PORTS[k].Name,
								this->PARTITION_VECTOR[counter].QUEUING_PORTS[k].MAX_MESSAGE_SIZE,
								this->PARTITION_VECTOR[counter].QUEUING_PORTS[k].MAX_NB_MESSAGE, 
								&RETURN_CODE);
							ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
							for (unsigned int i=0;i<iter->second.destination.size();i++)
							{
								std::string destination_name= iter->second.destination[i];
								QUEUING_PORT_DESCRIPTOR descriptor;
								unsigned int sizevec=0;
								bool foundinproxy=false;
								int j= FIND_PARTITION_FROM_QUEUING_PORT(destination_name,descriptor);
								if(j==-1) //check if name exists in a proxy_port
								{

									//check if name exists in a proxy_port and the direction of proxy port is destination...

									for(;sizevec<this->PROXY_PORTS.size();sizevec++)
									{
										if (PROXY_PORTS[sizevec].Name.compare(destination_name)==0)
										{
											foundinproxy=true;
											break;
										}
									}						

									if(!foundinproxy)
									{
										LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
										throw(-1);
									}
									else
									{
										if(PROXY_PORTS[sizevec].PORT_DIRECTION!=ACM_USER::DESTINATION)
										{
											LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
											PROXY_PORTS[sizevec].printout();
											throw(-1);

										}
									}
								}

								if(foundinproxy)
								{
									RETURN_CODE_TYPE return_code;
									if (channel->GET_COMMUNICATION_TYPE()!=ACM_INTERNAL::INTER_MODULE_SRC)
									{
										channel->SET_COMMUNICATION_TYPE_ENUM(ACM_INTERNAL::INTER_MODULE_SRC,&RETURN_CODE);
										ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
									}
									channel->ADD_DESTINATION_URL(PROXY_PORTS[sizevec].hostName,PROXY_PORTS[sizevec].portNumber,&return_code);
									ENSURE_CONDITION(RETURN_CODE==NO_ERROR);								
								}
								else if(j>=0)
								{
									if(descriptor.PORT_DIRECTION!=ACM_USER::DESTINATION)
									{
										LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
										descriptor.printout();
										throw(-1);
									}
									channel->ADD_DESTINATION_QUEUE(descriptor.Name,descriptor.MAX_MESSAGE_SIZE,descriptor.MAX_NB_MESSAGE, &RETURN_CODE);
									ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
								}							
							}						
							//create the channel
							channel->SET_INITIALIZED();

							this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR.push_back(channel->GET_NAME());
							this->MAP_OF_QUEUEING_CHANNELS[channel->GET_NAME()]=channel;

						}
						break;
					}
				}						
			}
		}



		//////////////// Inter Module Destination
		unsigned int sizevec=0;
		for(;sizevec<this->PROXY_PORTS.size();sizevec++)
		{
			if(PROXY_PORTS[sizevec].PORT_DIRECTION==ACM_USER::DESTINATION || PROXY_PORTS[sizevec].PROXY_TYPE==PROXY_PORT_DESCRIPTORS::SAMPLING ) continue;

			//this port is a source port
			CHANNEL_MAP_TYPE::iterator source_iterator;
			for (CHANNEL_MAP_TYPE::iterator iter=CHANNEL_MAP.begin();iter!=CHANNEL_MAP.end();iter++)
			{
				if (iter->second.source.compare(this->PROXY_PORTS[sizevec].Name)==0)
				{

					//check if channel is in the network partition list...

					if (CanAddChannel(iter->first))
					{

						QUEUE_CHANNEL * channel = new QUEUE_CHANNEL(std::string(iter->first),INTER_MODULE_DST,PROXY_PORTS[sizevec].portNumber); //channel name


						channel->SET_MAX_SIZE_OF_MESSAGE(PROXY_PORTS[sizevec].MAX_MESSAGE_SIZE,&RETURN_CODE);
						channel->SET_MAX_SOCKET_MESSAGE(PROXY_PORTS[sizevec].MAX_NB_MESSAGE,&RETURN_CODE);
						ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
						for (unsigned int i=0;i<iter->second.destination.size();i++)
						{
							std::string destination_name= iter->second.destination[i];
							QUEUING_PORT_DESCRIPTOR descriptor;
							//unsigned int sizevec=0;

							int j= FIND_PARTITION_FROM_QUEUING_PORT(destination_name,descriptor);
							if(j==-1) //check if name exists in a proxy_port
							{
								//check if name exists in a proxy_port and the direction of proxy port is destination...						
								LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
								throw(-1);

							}
							if(j>=0)
							{
								if(descriptor.PORT_DIRECTION!=ACM_USER::DESTINATION)
								{
									LOGGER(FATAL,"Problem creating channel with destination %s",destination_name.c_str());
									descriptor.printout();
									throw(-1);
								}
								channel->ADD_DESTINATION_QUEUE(descriptor.Name,descriptor.MAX_MESSAGE_SIZE,descriptor.MAX_NB_MESSAGE, &RETURN_CODE);

								bool alreadyInpartition=false;
								for(unsigned int counter=0;counter<this->PARTITION_VECTOR[j].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.size();counter++)
								{

									if(this->PARTITION_VECTOR[j].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[counter].compare(channel->GET_NAME())==0)
									{
										alreadyInpartition=true;
										break;
									}

								}
								if(!alreadyInpartition) this->PARTITION_VECTOR[j].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.push_back(channel->GET_NAME());
								ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
							}							
						}						
						//create the channel
						channel->SET_INITIALIZED();		


						this->MAP_OF_QUEUEING_CHANNELS[channel->GET_NAME()]=channel;

						//this->QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.push_back(channel);
						//this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR.push_back(channel);
						//this->PARTITION_VECTOR[counter].QUEUE_CHANNEL_SOURCE_VECTOR[currentsize].SET_INITIALIZED();
					}
					break;
				}
			}
		}
	}
	void MODULE_MANAGER::SET_MAJOR_FRAME_SIZE()
	{
		std::string SEARCH_STRING=PRIMARY_KEY_HYPERPERIOD_KEY;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		if(searchPair.first==searchPair.second)
		{
			LOGGER(FATAL,"incorrect configurations: HYPERPERIOD missing");
			throw(-2);		
		}
		std::string framestring=searchPair.first->second;		
		stringTrim(framestring," ");
		//int frameInt = atoi(framestring.c_str()); 
		double frameDouble=0;

		if(!from_string<double>(frameDouble,framestring,std::dec))
		{
			LOGGER(FATAL,"Incorrect HYPERPERIOD VALUE %s",framestring.c_str());
			throw(-2);
		}
		this->MAJOR_FRAME_SIZE=convertSecsToNano(frameDouble);		
		LOGGER(INFO,"Frame size is %lld",this->MAJOR_FRAME_SIZE);		
	}
	void MODULE_MANAGER::SET_MAX_NUMBER_OF_FRAMES()
	{

		std::string SEARCH_STRING=PRIMARY_KEY_MAXITERATIONS;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		if(searchPair.first==searchPair.second)
		{
			LOGGER(INFO,"No MAXITERATIONS Limit. Will run forever");
			return;			
		}


		std::string maxiterationstring=searchPair.first->second;		
		stringTrim(maxiterationstring," ");
		//int frameInt = atoi(framestring.c_str()); 



		if(!from_string<int>(this->MAX_NUMBER_OF_FRAMES,maxiterationstring,std::dec))
		{
			LOGGER(FATAL,"Incorrect MAXITERATIONS VALUE %s",maxiterationstring.c_str());
			throw(-2);
		}

		LOGGER(INFO,"MAXITERATIONS limit is %d",this->MAX_NUMBER_OF_FRAMES);	
		this->isMAX_NUMBER_OF_FRAMES_SET=true;
	}
	void MODULE_MANAGER::SET_MIN_WAIT_TIME()
	{

		std::string SEARCH_STRING=PRIMARY_KEY_PARTITION_INIT_TIMEOUT;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		if(searchPair.first==searchPair.second)
		{
			char bufstr[100];
			sprintf(bufstr,"No PARTITION_INIT_TIMEOUT Limit. Will wait for  %d", this->MIN_WAIT_TIME);
			LOGGER(INFO,bufstr);
			return;			
		}


		std::string minwaittimestring=searchPair.first->second;		
		stringTrim(minwaittimestring," ");



		if(!from_string<int>(this->MIN_WAIT_TIME,minwaittimestring,std::dec))
		{
			LOGGER(FATAL,"Incorrect PARTITION_INIT_TIMEOUT VALUE %s",minwaittimestring.c_str());
			throw(-2);
		}

		LOGGER(INFO,"PARTITION_INIT_TIMEOUT limit is %d",this->MIN_WAIT_TIME);
		this->isMIN_WAIT_TIME_SET=true;
	}
	void MODULE_MANAGER::SET_DISCOVERY_PROTOCOL()
	{

		std::string SEARCH_STRING=PRIMARY_KEY_DISCOVERY_PROTOCOL;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(SEARCH_STRING);
		char bufstr[100];
		if(searchPair.first==searchPair.second)
		{

			sprintf(bufstr,"No %s set. Will  use %s", PRIMARY_KEY_DISCOVERY_PROTOCOL,PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR);
			LOGGER(INFO,bufstr);
			return;			
		}

		std::string discoveryprotocol=searchPair.first->second;		
		stringTrim(discoveryprotocol," ");



		if(discoveryprotocol.compare(PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR)==0 )
		{
			sprintf(bufstr,"Setting %s to %s", PRIMARY_KEY_DISCOVERY_PROTOCOL,PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR);
			LOGGER(INFO,bufstr);
			DISCOVERY_PROTOCOL=IOR;
			return;
		}
		else if (discoveryprotocol.compare(PRIMARY_KEY_DISCOVERY_PROTOCOL_NS)==0 )
		{

			sprintf(bufstr,"Setting %s to %s", PRIMARY_KEY_DISCOVERY_PROTOCOL,PRIMARY_KEY_DISCOVERY_PROTOCOL_NS);
			LOGGER(INFO,bufstr);
			DISCOVERY_PROTOCOL=NAMINGSERVICE;


			std::string SEARCH_STRING1=PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST;
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair1;
			searchPair1=this->PropertyMap.equal_range(SEARCH_STRING1);
			if(searchPair1.first==searchPair1.second)
			{
				char bufstr[100];
				sprintf(bufstr,"You have set %s to %s but not provided value for %s", PRIMARY_KEY_DISCOVERY_PROTOCOL,PRIMARY_KEY_DISCOVERY_PROTOCOL_NS,PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST);
				LOGGER(FATAL,bufstr);
				throw(-3);		
			}
			else
			{	
				this->NS_HOST_IP=searchPair1.first->second;		
				stringTrim(this->NS_HOST_IP," ");
				LOGGER(INFO,"Setting %s to %s",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST,this->NS_HOST_IP.c_str());
			}

			SEARCH_STRING1=PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT;
			searchPair1=this->PropertyMap.equal_range(SEARCH_STRING1);
			if(searchPair1.first==searchPair1.second)
			{
				char bufstr[100];
				sprintf(bufstr,"You have set %s to %s but not provided value for %s", PRIMARY_KEY_DISCOVERY_PROTOCOL,PRIMARY_KEY_DISCOVERY_PROTOCOL_NS,PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT);
				LOGGER(FATAL,bufstr);
				throw(-3);		
			}
			else
			{	
				std::string  port=searchPair1.first->second;		
				stringTrim(port," ");
				if(!from_string<int>(this->NS_PORT_NUMBER,port,std::dec))
				{
					LOGGER(FATAL,"Incorrect%s %s",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT,port.c_str());
					throw(-2);
				}
				if(this->NS_PORT_NUMBER<=0)
				{
					LOGGER(FATAL,"Incorrect%s %s",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT,port.c_str());						
					throw(-2);
				}

				LOGGER(INFO,"Setting %s to %d",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT,this->NS_PORT_NUMBER);		

				return;
			}

		}
		else
		{
			LOGGER(FATAL,"Wrong choice %s for %s. Correct choices are %s and %s",discoveryprotocol.c_str(),PRIMARY_KEY_DISCOVERY_PROTOCOL,PRIMARY_KEY_DISCOVERY_PROTOCOL_NS,PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR);
			throw(-3);			
		}
	}
	void MODULE_MANAGER::CREATE_PROXY_PORT_DESCRIPTORS()
	{
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(PRIMARY_KEY_PROXY_PORT);
		PROXY_PORT_DESCRIPTORS PROXY_PORT_DESCRIPTOR;
		for(PropertyMapIteratorType iter =searchPair.first; iter !=searchPair.second;iter++)
		{
			std::string PROXY_NAME=iter->second;
			PROXY_PORT_DESCRIPTOR.Name=PROXY_NAME;
			//find Module Name
			std::string ModuleName =PROXY_NAME+ADDITIVE_KEY__MODULE_NAME;
			std::string Portnum =PROXY_NAME+ADDITIVE_KEY__PORT_NUMBER;
			std::string messagemaxsize=PROXY_NAME+ADDITIVE_KEY__MAXMESSAGESIZE;
			std::string DIRECTION_String=PROXY_NAME+ADDITIVE_KEY__DIRECTION;
			std::string messagemax_num=PROXY_NAME+ADDITIVE_KEY__MAXNUMBEROFMESSAGES;

			std::string proxytypestr=PROXY_NAME+ADDITIVE_KEY__TYPE;

			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> ModuleName_PROPERTY_SEARCH;
			ModuleName_PROPERTY_SEARCH=this->PropertyMap.equal_range(ModuleName);
			if (ModuleName_PROPERTY_SEARCH.first==ModuleName_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",ModuleName.c_str());
				throw(-3);
			}
			PROXY_PORT_DESCRIPTOR.hostName=ModuleName_PROPERTY_SEARCH.first->second;



			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> porttype_PROPERTY_SEARCH;
			porttype_PROPERTY_SEARCH=this->PropertyMap.equal_range(proxytypestr);

			if (porttype_PROPERTY_SEARCH.first==porttype_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",proxytypestr.c_str());
				throw(-3);
			}

			std::string proxytypetmp=porttype_PROPERTY_SEARCH.first->second;
			if(proxytypetmp.compare(PROXY_TYPE_SAMPLING)==0)
			{
				PROXY_PORT_DESCRIPTOR.PROXY_TYPE=PROXY_PORT_DESCRIPTORS::SAMPLING;
			}
			else if(proxytypetmp.compare(PROXY_TYPE_QUEUEING)==0)
			{
				PROXY_PORT_DESCRIPTOR.PROXY_TYPE=PROXY_PORT_DESCRIPTORS::QUEUEING;
			}
			else
			{
				LOGGER(FATAL,"Cannot resolve %s=%s",proxytypestr.c_str(),proxytypetmp.c_str());
				throw(-3);
			}




			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> Portnum_PROPERTY_SEARCH;
			Portnum_PROPERTY_SEARCH=this->PropertyMap.equal_range(Portnum);
			if (Portnum_PROPERTY_SEARCH.first==Portnum_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",Portnum.c_str());
				throw(-3);
			}	

			std::string Portnumtemp=Portnum_PROPERTY_SEARCH.first->second;			
			if(!from_string<unsigned int>(PROXY_PORT_DESCRIPTOR.portNumber,Portnumtemp,std::dec))
			{
				LOGGER(FATAL,"Incorrect Port number %s for proxyPort %s",Portnumtemp.c_str(),PROXY_NAME.c_str());
				throw(-3);
			}

			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> messagemaxsize_PROPERTY_SEARCH;
			messagemaxsize_PROPERTY_SEARCH=this->PropertyMap.equal_range(messagemaxsize);
			if (messagemaxsize_PROPERTY_SEARCH.first==messagemaxsize_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",messagemaxsize.c_str());
				throw(-3);
			}	

			messagemaxsize=messagemaxsize_PROPERTY_SEARCH.first->second;			
			if(!from_string<MESSAGE_SIZE_TYPE>(PROXY_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,messagemaxsize,std::dec))
			{
				LOGGER(FATAL,"Incorrect max message size %s for proxyPort %s",messagemaxsize.c_str(),PROXY_NAME.c_str());
				throw(-3);

			}
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> messagemax_num_PROPERTY_SEARCH;
			messagemax_num_PROPERTY_SEARCH=this->PropertyMap.equal_range(messagemax_num);
			if (messagemax_num_PROPERTY_SEARCH.first==messagemax_num_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",messagemax_num.c_str());
				throw(-3);
			}	

			messagemax_num=messagemax_num_PROPERTY_SEARCH.first->second;			
			if(!from_string<MESSAGE_RANGE_TYPE>(PROXY_PORT_DESCRIPTOR.MAX_NB_MESSAGE,messagemax_num,std::dec))
			{
				LOGGER(FATAL,"Incorrect max message size %s for proxyPort %s",messagemax_num.c_str(),PROXY_NAME.c_str());
				throw(-3);

			}

			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> Direction_PROPERTIES_SEARCH_PAIR;
			Direction_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(DIRECTION_String);
			if (Direction_PROPERTIES_SEARCH_PAIR.first==Direction_PROPERTIES_SEARCH_PAIR.second)
			{
				LOGGER(FATAL,"Cannot find %s",DIRECTION_String.c_str());
				throw(-3);
			}
			std::string DIRECTION_data=Direction_PROPERTIES_SEARCH_PAIR.first->second;
			if (DIRECTION_data.compare("SOURCE")==0){
				PROXY_PORT_DESCRIPTOR.PORT_DIRECTION=ACM_USER::SOURCE;					
			}
			else if (DIRECTION_data.compare("DESTINATION")==0) 
			{
				PROXY_PORT_DESCRIPTOR.PORT_DIRECTION=ACM_USER::DESTINATION;
			}
			else
			{
				LOGGER(ERROR,"WRONG TOken %s in proxy port %s_DIRECTION",DIRECTION_data.c_str(),PROXY_NAME.c_str());			
				throw(-3);
			}

			//name5_DIRECTION= SOURCE
			PROXY_PORT_DESCRIPTOR.printout();
			this->PROXY_PORTS.push_back(PROXY_PORT_DESCRIPTOR);
		}

	}
	void MODULE_MANAGER::CREATE_CHANNEL_DSCRIPTOR()
	{
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(PRIMARY_KEY_CHANNEL_NAME);
		for(PropertyMapIteratorType iter =searchPair.first; iter !=searchPair.second;iter++)
		{
			std::string channelName=iter->second;

			//find source
			std::string SOURCE_STRING =channelName+ADDITIVE_KEY__SOURCE;
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> SOURCE_STRING_PROPERTY_SEARCH;
			SOURCE_STRING_PROPERTY_SEARCH=this->PropertyMap.equal_range(SOURCE_STRING);

			if (SOURCE_STRING_PROPERTY_SEARCH.first==SOURCE_STRING_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",SOURCE_STRING.c_str());
				throw(-3);
			}	

			std::string channelSource=SOURCE_STRING_PROPERTY_SEARCH.first->second;

			std::string DIRECTION_String=channelSource+ADDITIVE_KEY__DIRECTION;
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType>  PORT_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(DIRECTION_String);

			if (PORT_PROPERTIES_SEARCH_PAIR.first==PORT_PROPERTIES_SEARCH_PAIR.second)
			{
				LOGGER(FATAL,"Cannot find %s",DIRECTION_String.c_str());
				throw(-3);
			}
			std::string DIRECTION_data=PORT_PROPERTIES_SEARCH_PAIR.first->second;

			if (DIRECTION_data.compare("SOURCE")!=0)
			{
				LOGGER(FATAL,"The port configured for %s should have SOURCE DIRECTION",SOURCE_STRING.c_str());
				throw(-3);

			}


			//Now find the property of this port

			std::string DESTINATION_STRING =channelName+ADDITIVE_KEY__DESTINATION;
			SOURCE_STRING_PROPERTY_SEARCH=this->PropertyMap.equal_range(DESTINATION_STRING);
			if (SOURCE_STRING_PROPERTY_SEARCH.first==SOURCE_STRING_PROPERTY_SEARCH.second)
			{
				LOGGER(FATAL,"Cannot find %s",DESTINATION_STRING.c_str());
				throw(-3);
			}	
			std::vector<std::string> channelDestination;

			for(PropertyMapIteratorType iter_inner=SOURCE_STRING_PROPERTY_SEARCH.first;iter_inner!=SOURCE_STRING_PROPERTY_SEARCH.second;iter_inner++)
			{
				std::string temp =iter_inner->second;
				channelDestination.push_back(temp);
				std::string DIRECTION_String=temp+ADDITIVE_KEY__DIRECTION;
				std::pair<PropertyMapIteratorType,PropertyMapIteratorType>  PORT_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(DIRECTION_String);

				if (PORT_PROPERTIES_SEARCH_PAIR.first==PORT_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot find %s",DIRECTION_String.c_str());
					throw(-3);
				}
				std::string DIRECTION_data=PORT_PROPERTIES_SEARCH_PAIR.first->second;

				if (DIRECTION_data.compare("DESTINATION")!=0)
				{
					LOGGER(FATAL,"The port %s configured for %s should have DESTINATION DIRECTION",temp.c_str(),DESTINATION_STRING.c_str());
					throw(-3);

				}

			}
			CHANNEL_DESCRIPTOR tempCHANNEL_DESCRIPTOR;
			tempCHANNEL_DESCRIPTOR.destination=channelDestination;
			tempCHANNEL_DESCRIPTOR.source=channelSource;
			CHANNEL_MAP[channelName]=tempCHANNEL_DESCRIPTOR;
		}
		for (CHANNEL_MAP_TYPE::iterator iter=CHANNEL_MAP.begin();iter!=CHANNEL_MAP.end();iter++)
		{
			for (unsigned int i=0;i<iter->second.destination.size();i++)
			{
				LOGGER(INFO,"Channel %s, SourcePort:%s, DestinationPort:%s",iter->first.c_str(),iter->second.source.c_str(),iter->second.destination[i].c_str());
			}
		}
	}
	void MODULE_MANAGER::CREATE_NETWORK_PARTITION_VECTOR()
	{
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;	
		searchPair=this->PropertyMap.equal_range(PRIMARY_KEY_NETWORK_PARTITION_NAME);

		for(PropertyMapIteratorType iter =searchPair.first; iter !=searchPair.second;iter++)
		{
			struct PARTITION temporaryPartition;
			unsigned int tempPartitionIndex=this->PARTITION_VECTOR.size();
			this->PARTITION_VECTOR.push_back(temporaryPartition);
			this->PARTITION_VECTOR[tempPartitionIndex].Name=iter->second;
			this->PARTITION_VECTOR[tempPartitionIndex].isNetworkPartition=true;

#ifndef ISNETWORK_PARTITION
			//create configuration queue
			RETURN_CODE_TYPE queuing_return_code;
			QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME;
			snprintf(QUEUING_PORT_NAME,MAX_NAME_LENGTH,"%s_config",this->PARTITION_VECTOR[tempPartitionIndex].Name.c_str());



			MESSAGE_QUEUE_TYPE* temp_queue=	new MESSAGE_QUEUE_TYPE  (QUEUING_PORT_NAME,
				sizeof(NETWORK_PARTITION_PETRI_NET_TOKEN_TYPE),
				10,
				PORT_SOURCE_DEST,&queuing_return_code,
				O_CREAT|O_EXCL); 
			if(queuing_return_code!=NO_ERROR)
			{
				LOGGER(FATAL,"Error in Creating Queue %s. Please clean /dev/mqeueue",QUEUING_PORT_NAME);
				LOGGER(FATAL,"To Mount: mkdir /dev/mqueue; mount -t mqueue none /dev/mqueue");
				throw(-3);
			}
			ENSURE_CONDITION(queuing_return_code==NO_ERROR);


			this->PARTITION_VECTOR[tempPartitionIndex].CONFIGURATION_QUEUE = QUEUING_PORT_NAME;
			this->MAP_OF_CONFIGURATION_QUEUE[this->PARTITION_VECTOR[tempPartitionIndex].CONFIGURATION_QUEUE]=temp_queue;
#endif




			LOGGER(INFO,"**********Created %s",iter->second.c_str());
			//Channels;

			std::string channel_name_search_string=this->PARTITION_VECTOR[tempPartitionIndex].Name;
			channel_name_search_string.append(ADDITVE_KEY_CHANNEL);
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> CHANNEL_SEARCH_PAIR;
			CHANNEL_SEARCH_PAIR=this->PropertyMap.equal_range(channel_name_search_string);

			for (PropertyMapIteratorType PropertyMapIterator=CHANNEL_SEARCH_PAIR.first;PropertyMapIterator!=CHANNEL_SEARCH_PAIR.second;PropertyMapIterator++)
			{
				std::string channel_name=PropertyMapIterator->second;
				LOGGER(INFO,"**********Cchannel %s",channel_name.c_str());
				std::string source_port = CHANNEL_MAP[channel_name].source;

				std::pair<PropertyMapIteratorType,PropertyMapIteratorType> search_port_string_SEARCH_PAIR;
				std::string search_port_string=source_port+ADDITIVE_KEY__TYPE;
				search_port_string_SEARCH_PAIR=this->PropertyMap.equal_range(search_port_string);

				if(search_port_string_SEARCH_PAIR.first==search_port_string_SEARCH_PAIR.second)
				{

					search_port_string=source_port+ADDITIVE_KEY__REFRESHPERIOD;
					search_port_string_SEARCH_PAIR=this->PropertyMap.equal_range(search_port_string);
					if(search_port_string_SEARCH_PAIR.first==search_port_string_SEARCH_PAIR.second)
					{
						this->PARTITION_VECTOR[tempPartitionIndex].QUEUE_CHANNEL_SOURCE_VECTOR.push_back(channel_name);
					}
					else
					{

						this->PARTITION_VECTOR[tempPartitionIndex].SAMPLING_CHANNEL_SOURCE_VECTOR.push_back(channel_name);
					}
				}
				else
				{
					std::string value=search_port_string_SEARCH_PAIR.first->second;
					if (value.compare(PROXY_TYPE_SAMPLING)==0)
					{
						this->PARTITION_VECTOR[tempPartitionIndex].SAMPLING_CHANNEL_SOURCE_VECTOR.push_back(channel_name);
					}
					else if (value.compare(PROXY_TYPE_QUEUEING)==0)
					{
						this->PARTITION_VECTOR[tempPartitionIndex].QUEUE_CHANNEL_SOURCE_VECTOR.push_back(channel_name);
					}
					else
					{
						LOGGER(ERROR,"Incorrect %s=%s",search_port_string.c_str(),value.c_str());
						throw(-3);
					}
				}
				//search through 
				//name5_TYPE
				//name2_REFRESHPERIOD= 1
				//


			}

			//SCHEDULE
			std::string sched_name_string=this->PARTITION_VECTOR[tempPartitionIndex].Name;
			sched_name_string.append(ADDITIVE_KEY__SCHEDULE);
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> SCHEDULE_SEARCH_PAIR;
			SCHEDULE_SEARCH_PAIR = this->PropertyMap.equal_range(sched_name_string);
			if (SCHEDULE_SEARCH_PAIR.first==SCHEDULE_SEARCH_PAIR.second)
			{
				LOGGER(FATAL,"Cannot find %s",sched_name_string.c_str());
				throw(-3);
			}		
			for (PropertyMapIteratorType PropertyMapIterator=SCHEDULE_SEARCH_PAIR.first;PropertyMapIterator!=SCHEDULE_SEARCH_PAIR.second;PropertyMapIterator++)
			{
				if (PropertyMapIterator==this->PropertyMap.end())
				{
					LOGGER(FATAL,"Cannot find %s",sched_name_string.c_str());
					throw(-3);
				}
				std::string schedule_string=PropertyMapIterator->second;
				std::vector <std::string> tokens;
				stringTokenize(schedule_string,tokens,",");
				if (tokens.size()!=2)
				{
					LOGGER(FATAL,"Problem with %s=%s",sched_name_string.c_str(),schedule_string.c_str());
					throw(-3);
				}
				SCHEDULING_TUPLE tuple;
				double OFFSET =0;
				if(!from_string<double>(OFFSET,tokens[0],std::dec))
				{
					LOGGER(FATAL,"ERROR parsing %s to double",tokens[0].c_str());
					throw(-4);
				}
				LOGGER(INFO,"Read OFFSET Value is %4.9f",OFFSET);
				tuple.OFFSET = convertSecsToNano(OFFSET);

				double DURATION =0;
				if(!from_string<double>(DURATION,tokens[1],std::dec))
				{
					LOGGER(FATAL,"ERROR parsing %s to double",tokens[1].c_str());
					throw(-4);
				}

				LOGGER(DEBUG,"Read Duration Value is %4.9f",DURATION);
				tuple.DURATION = convertSecsToNano(DURATION);
				LOGGER(DEBUG,"TUPLE OFFSET is %lld, TUPLE DURATION is DURATION %lld",tuple.OFFSET,	tuple.DURATION );
				tuple.PARTITION_INDEX=tempPartitionIndex;
				this->SCHEDULING_VECTOR.push_back(tuple);				
			}	


			//executable

			char *ACM_ROOT = getenv("ACM_ROOT");

			this->PARTITION_VECTOR[tempPartitionIndex].Executable=ACM_ROOT;
			this->PARTITION_VECTOR[tempPartitionIndex].Executable.append("//bin//NetworkPartition");
		//	this->PARTITION_VECTOR[tempPartitionIndex].Executable.append(NETWORK_PART_EXEC);





		}		
		//PRIMARY_KEY_NETWORK_PARTITION_NAME
	}
	void MODULE_MANAGER::CREATE_PARTITION_VECTOR()
	{

		CREATE_PROXY_PORT_DESCRIPTORS();
		CREATE_CHANNEL_DSCRIPTOR();
		CREATE_NETWORK_PARTITION_VECTOR();




		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;	
		searchPair=this->PropertyMap.equal_range(PRIMARY_KEY_PARTITION_NAME);
		//Executable Name
		for(PropertyMapIteratorType iter =searchPair.first; iter !=searchPair.second;iter++)
		{
			struct PARTITION temporaryPartition;
			temporaryPartition.PID=0;
			unsigned int tempPartitionIndex=this->PARTITION_VECTOR.size();
			this->PARTITION_VECTOR.push_back(temporaryPartition);
			this->PARTITION_VECTOR[tempPartitionIndex].Name=iter->second;
			std::string exec_name_string=this->PARTITION_VECTOR[tempPartitionIndex].Name;
			exec_name_string.append(ADDITIVE_KEY__EXECUTABLE);
			PropertyMapIteratorType PropertyMapIterator = this->PropertyMap.find(exec_name_string);
			if (PropertyMapIterator==this->PropertyMap.end())
			{
				LOGGER(FATAL,"Cannot find %s",exec_name_string.c_str());
				throw(-3);
			}
			this->PARTITION_VECTOR[tempPartitionIndex].Executable=PropertyMapIterator->second;	

			//Check if the partition Executable exists and is readable.

			if (!fexists(this->PARTITION_VECTOR[tempPartitionIndex].Executable.c_str()))
			{
				LOGGER(FATAL,"Cannot access %s",this->PARTITION_VECTOR[tempPartitionIndex].Executable.c_str());
				throw(-3);

			}

			//SCHEDULE
			std::string sched_name_string=this->PARTITION_VECTOR[tempPartitionIndex].Name;
			sched_name_string.append(ADDITIVE_KEY__SCHEDULE);
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> SCHEDULE_SEARCH_PAIR;
			SCHEDULE_SEARCH_PAIR = this->PropertyMap.equal_range(sched_name_string);
			if (SCHEDULE_SEARCH_PAIR.first==SCHEDULE_SEARCH_PAIR.second)
			{
				LOGGER(FATAL,"Cannot find %s",sched_name_string.c_str());
				throw(-3);
			}		
			for (PropertyMapIterator=SCHEDULE_SEARCH_PAIR.first;PropertyMapIterator!=SCHEDULE_SEARCH_PAIR.second;PropertyMapIterator++)
			{
				if (PropertyMapIterator==this->PropertyMap.end())
				{
					LOGGER(FATAL,"Cannot find %s",sched_name_string.c_str());
					throw(-3);
				}
				std::string schedule_string=PropertyMapIterator->second;
				std::vector <std::string> tokens;
				stringTokenize(schedule_string,tokens,",");
				if (tokens.size()!=2)
				{
					LOGGER(FATAL,"Problem with %s=%s",sched_name_string.c_str(),schedule_string.c_str());
					throw(-3);
				}
				SCHEDULING_TUPLE tuple;
				double OFFSET =0;
				if(!from_string<double>(OFFSET,tokens[0],std::dec))
				{
					LOGGER(FATAL,"ERROR parsing %s to int",tokens[0].c_str());
					throw(-4);
				}
				LOGGER(DEBUG,"Before Sending to Convert to NanoSec OFFSET Value is %4.9f",OFFSET);
				tuple.OFFSET = convertSecsToNano(OFFSET);
				LOGGER(DEBUG,"Read OFFSET Value is %4.9f",OFFSET);
				double DURATION =0;
				if(!from_string<double>(DURATION,tokens[1],std::dec))
				{
					LOGGER(FATAL,"ERROR parsing %s to int",tokens[1].c_str());
					throw(-4);
				}
				LOGGER(DEBUG,"Read Duration Value is %4.9f",DURATION);
				tuple.DURATION = convertSecsToNano(DURATION);				
				LOGGER(INFO,"TUPLE OFFSET is %lld, TUPLE DURATION is DURATION %lld",tuple.OFFSET,	tuple.DURATION );
				tuple.PARTITION_INDEX=tempPartitionIndex;
				this->SCHEDULING_VECTOR.push_back(tuple);				
			}
			//////////////////////////////////////SAMPLING_PORT////////////////////////////
			std::string sampling_string=this->PARTITION_VECTOR[tempPartitionIndex].Name;
			sampling_string.append(ADDITIVE_KEY__SAMPLINGPORT);
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType> SAMPLING_SEARCH_PAIR;
			SAMPLING_SEARCH_PAIR = this->PropertyMap.equal_range(sampling_string);			
			//LOGGER(INFO,"sampling_string is %s",sampling_string.c_str());
			//exit(-1);			
			for (PropertyMapIteratorType iter =SAMPLING_SEARCH_PAIR.first;iter!=SAMPLING_SEARCH_PAIR.second;iter++)
			{
				std::string sampling_port_name=iter->second;
				//LOGGER(INFO,"Sampling port name %s",sampling_port_name.c_str());
				SAMPLING_PORT_DESCRIPTOR temp_SAMPLING_PORT_DESCRIPTOR;
				temp_SAMPLING_PORT_DESCRIPTOR.Name=sampling_port_name;
				std::string MAXMESSAGESIZE_String=sampling_port_name+ADDITIVE_KEY__MAXMESSAGESIZE;
				std::pair<PropertyMapIteratorType,PropertyMapIteratorType> SAMPLING_PROPERTIES_SEARCH_PAIR;
				SAMPLING_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(MAXMESSAGESIZE_String);
				if (SAMPLING_PROPERTIES_SEARCH_PAIR.first==SAMPLING_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot find %s",MAXMESSAGESIZE_String.c_str());
					throw(-3);
				}
				std::string max_message_data=SAMPLING_PROPERTIES_SEARCH_PAIR.first->second;

				if(!from_string<MESSAGE_SIZE_TYPE>(temp_SAMPLING_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,max_message_data,std::dec))
				{
					LOGGER(INFO,"ERROR parsing %s to int",max_message_data.c_str());
					throw(-4);
				}

				std::string REFRESHPERIOD_String=sampling_port_name+ADDITIVE_KEY__REFRESHPERIOD;
				SAMPLING_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(REFRESHPERIOD_String);
				if (SAMPLING_PROPERTIES_SEARCH_PAIR.first==SAMPLING_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot find %s",REFRESHPERIOD_String.c_str());
					throw(-3);
				}
				std::string refresh_data=SAMPLING_PROPERTIES_SEARCH_PAIR.first->second;

				double refresh_period=0;
				if(!from_string<double>(refresh_period,refresh_data,std::dec))
				{
					LOGGER(INFO,"ERROR parsing %s to int",refresh_data.c_str());
					throw(-4);
				}
				temp_SAMPLING_PORT_DESCRIPTOR.REFRESH_PERIOD=convertSecsToNano(refresh_period);
				std::string DIRECTION_String=sampling_port_name+ADDITIVE_KEY__DIRECTION;
				SAMPLING_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(DIRECTION_String);
				if (SAMPLING_PROPERTIES_SEARCH_PAIR.first==SAMPLING_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot find %s",DIRECTION_String.c_str());
					throw(-3);
				}
				std::string DIRECTION_data=SAMPLING_PROPERTIES_SEARCH_PAIR.first->second;

				if (DIRECTION_data.compare("SOURCE")==0){
					temp_SAMPLING_PORT_DESCRIPTOR.PORT_DIRECTION=ACM_USER::SOURCE;					
				}
				else if (DIRECTION_data.compare("DESTINATION")==0) 
				{
					temp_SAMPLING_PORT_DESCRIPTOR.PORT_DIRECTION=ACM_USER::DESTINATION;
				}
				else
				{
					LOGGER(ERROR,"WRONG TOken %s in sampling port %s_DIRECTION",DIRECTION_data.c_str(),temp_SAMPLING_PORT_DESCRIPTOR.Name.c_str());			
					throw(-4);
				}
				//DONE WIth Sampling PORTS
				//	LOGGER(INFO,"DONE WITH SAMPLING PORTS");

				{

#ifndef ISNETWORK_PARTITION
					RETURN_CODE_TYPE sampling_return_code;

					if(killfile.good() && killfile.is_open())
					{

						killfile<<"rm -f /dev/shm/"<<temp_SAMPLING_PORT_DESCRIPTOR.Name<<std::endl;					
						killfile.flush();
					}


					SAMPLING_PORT_HELPER tmpPort (temp_SAMPLING_PORT_DESCRIPTOR.Name,
						temp_SAMPLING_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,
						PORT_SOURCE_DEST,
						temp_SAMPLING_PORT_DESCRIPTOR.REFRESH_PERIOD,
						&sampling_return_code,
						O_CREAT|O_EXCL);

					if(sampling_return_code!=NO_ERROR)
					{
						LOGGER(FATAL,"Error in creating sampling port %s",temp_SAMPLING_PORT_DESCRIPTOR.Name.c_str());		
						throw(-4);
					}
					//ENSURE_CONDITION(sampling_return_code==NO_ERROR);
#else
					/*SAMPLING_PORT_HELPER tmpPort (temp_SAMPLING_PORT_DESCRIPTOR.Name,
					temp_SAMPLING_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,
					PORT_SOURCE_DEST,
					temp_SAMPLING_PORT_DESCRIPTOR.REFRESH_PERIOD,
					&sampling_return_code,
					O_CREAT);*/
#endif


				}

				this->PARTITION_VECTOR[tempPartitionIndex].SAMPLING_PORTS.push_back(temp_SAMPLING_PORT_DESCRIPTOR);	

				//check if Samplping Port Can be Created Properly or Not.




			}


			/////////////////////////// QUEUING PORTS////////////////////////////////////////

			std::string queuing_string=this->PARTITION_VECTOR[tempPartitionIndex].Name;
			queuing_string.append(ADDITIVE_KEY__QUEUINGPORT);
			//std::cout<<queuing_string<<std::endl;
			std::pair<PropertyMapIteratorType,PropertyMapIteratorType>QUEUING_SEARCH_PAIR;
			QUEUING_SEARCH_PAIR = this->PropertyMap.equal_range(queuing_string);
			for (PropertyMapIteratorType iter =QUEUING_SEARCH_PAIR.first;iter!=QUEUING_SEARCH_PAIR.second;iter++)
			{
				std::string queuing_port_name=iter->second;
				QUEUING_PORT_DESCRIPTOR temp_QUEUING_PORT_DESCRIPTOR;
				temp_QUEUING_PORT_DESCRIPTOR.Name=queuing_port_name;
				std::string MAXMESSAGESIZE_String=queuing_port_name+ADDITIVE_KEY__MAXMESSAGESIZE;
				std::pair<PropertyMapIteratorType,PropertyMapIteratorType> QUEUING_PROPERTIES_SEARCH_PAIR;
				QUEUING_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(MAXMESSAGESIZE_String);
				if (QUEUING_PROPERTIES_SEARCH_PAIR.first==QUEUING_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot  find %s",MAXMESSAGESIZE_String.c_str());
					throw(-3);
				}
				std::string max_message_data=QUEUING_PROPERTIES_SEARCH_PAIR.first->second;

				//std::cout<<max_message_data<<std::endl;

				if(!from_string<MESSAGE_SIZE_TYPE>(temp_QUEUING_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,max_message_data,std::dec))
				{
					LOGGER(INFO,"ERROR parsing %s to int",max_message_data.c_str());
					throw(-4);
				}
				std::string MAXNUMBEROFMESSAGES_String=queuing_port_name+ADDITIVE_KEY__MAXNUMBEROFMESSAGES;

				//std::cout<<"MAXNUMBEROFMESSAGES_String "<<MAXNUMBEROFMESSAGES_String<<std::endl;
				QUEUING_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(MAXNUMBEROFMESSAGES_String);

				if (QUEUING_PROPERTIES_SEARCH_PAIR.first==QUEUING_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot find %s",MAXNUMBEROFMESSAGES_String.c_str());
					throw(-3);
				}
				std::string MAXNUMBEROFMESSAGES_String_data=QUEUING_PROPERTIES_SEARCH_PAIR.first->second;
				//std::cout<<MAXNUMBEROFMESSAGES_String_data <<" MAXNUMBEROFMESSAGES_String_data" <<std::endl;

				if(!from_string<MESSAGE_RANGE_TYPE>(temp_QUEUING_PORT_DESCRIPTOR.MAX_NB_MESSAGE,MAXNUMBEROFMESSAGES_String_data,std::dec))
				{
					LOGGER(INFO,"ERROR parsing %s to int",MAXNUMBEROFMESSAGES_String_data.c_str());
					throw(-4);
				}

				std::string DIRECTION_String=queuing_port_name+ADDITIVE_KEY__DIRECTION;

				QUEUING_PROPERTIES_SEARCH_PAIR=this->PropertyMap.equal_range(DIRECTION_String);
				if (QUEUING_PROPERTIES_SEARCH_PAIR.first==QUEUING_PROPERTIES_SEARCH_PAIR.second)
				{
					LOGGER(FATAL,"Cannot find %s",DIRECTION_String.c_str());
					throw(-3);
				}
				std::string DIRECTION_data=QUEUING_PROPERTIES_SEARCH_PAIR.first->second;

				if (DIRECTION_data.compare("SOURCE")==0){
					temp_QUEUING_PORT_DESCRIPTOR.PORT_DIRECTION=ACM_USER::SOURCE;
				}
				else if (DIRECTION_data.compare("DESTINATION")==0) 
				{
					temp_QUEUING_PORT_DESCRIPTOR.PORT_DIRECTION=ACM_USER::DESTINATION;
				}
				else
				{
					LOGGER(ERROR,"WRONG TOken %s in sampling port %s_DIRECTION",DIRECTION_data.c_str(),temp_QUEUING_PORT_DESCRIPTOR.Name.c_str());
					throw(-4);
				}



				{
#ifndef ISNETWORK_PARTITION

					RETURN_CODE_TYPE queuing_return_code=NO_ERROR;
					//Ensure that the queue does not exist...
					if(killfile.good() && killfile.is_open())
					{
						killfile<<"rm -f /dev/mqueue/"<<temp_QUEUING_PORT_DESCRIPTOR.Name<<std::endl;					
						killfile.flush();
					}
					MESSAGE_QUEUE_TYPE  tmpPort (temp_QUEUING_PORT_DESCRIPTOR.Name,
						temp_QUEUING_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,
						temp_QUEUING_PORT_DESCRIPTOR.MAX_NB_MESSAGE,
						PORT_SOURCE_DEST,&queuing_return_code,
						O_CREAT|O_EXCL);
					if(queuing_return_code!=NO_ERROR)
					{
						LOGGER(FATAL,"Error in Creating Queue %s. Please clean /dev/mqeueue",temp_QUEUING_PORT_DESCRIPTOR.Name.c_str());
						LOGGER(FATAL,"To Mount: mkdir /dev/mqueue; mount -t mqueue none /dev/mqueue");
						throw(-3);
					}
					//	ENSURE_CONDITION(queuing_return_code==NO_ERROR);
#else
					/*		MESSAGE_QUEUE_TYPE  tmpPort (temp_QUEUING_PORT_DESCRIPTOR.Name,
					temp_QUEUING_PORT_DESCRIPTOR.MAX_MESSAGE_SIZE,
					temp_QUEUING_PORT_DESCRIPTOR.MAX_NB_MESSAGE,
					PORT_SOURCE_DEST,&queuing_return_code,
					O_CREAT);*/
#endif


				}
				this->PARTITION_VECTOR[tempPartitionIndex].QUEUING_PORTS.push_back(temp_QUEUING_PORT_DESCRIPTOR);

			}
		}
		this->CREATE_QUEUEING_CHANNELS();
		this->CREATE_SAMPLING_CHANNELS();
	}
	int MODULE_MANAGER::FIND_PARTITION_FROM_SAMPLING_PORT(std::string& PORT_NAME,SAMPLING_PORT_DESCRIPTOR& descriptor)
	{
		int return_value=-1;

		for (unsigned int counter=0;counter< this->PARTITION_VECTOR.size();counter++)
		{
			for( unsigned int i=0; i< this->PARTITION_VECTOR[counter].SAMPLING_PORTS.size();i++)
			{
				if (this->PARTITION_VECTOR[counter].SAMPLING_PORTS[i].Name.compare(PORT_NAME)==0)
				{
					//std::cout<< "          FOUUUUND "<<i<<"       "<<PORT_NAME<<std::endl;
					descriptor=this->PARTITION_VECTOR[counter].SAMPLING_PORTS[i];
					//std::cout<<descriptor.Name << descriptor.MAX_MESSAGE_SIZE <<descriptor.PORT_DIRECTION<<descriptor.REFRESH_PERIOD<<std::endl;
					return counter;
				}
			}
		}

		return return_value;

	}
	int MODULE_MANAGER::FIND_PARTITION_FROM_QUEUING_PORT(std::string& PORT_NAME,QUEUING_PORT_DESCRIPTOR& descriptor )
	{
		int return_value=-1;

		for (unsigned int counter=0;counter< this->PARTITION_VECTOR.size();counter++)
		{
			for( unsigned int i=0; i< this->PARTITION_VECTOR[counter].QUEUING_PORTS.size();i++)
			{
				if (this->PARTITION_VECTOR[counter].QUEUING_PORTS[i].Name.compare(PORT_NAME)==0)
				{
					descriptor=this->PARTITION_VECTOR[counter].QUEUING_PORTS[i];
					return counter;
				}
			}
		}

		return return_value;

	}

	void MODULE_MANAGER::SET_CPU_NUMBER()
	{
		std::string CPU_STRING=PRIMARY_KEY_CPU;
		std::pair<PropertyMapIteratorType,PropertyMapIteratorType> searchPair;
		searchPair=this->PropertyMap.equal_range(CPU_STRING);
		if(searchPair.first==searchPair.second)
		{
			LOGGER(INFO,"MODULE WILL SET Its Affinity to Default CPU %d",this->cpunumber );

		}
		else
		{
			std::string data =searchPair.first->second;
			stringTrim(data," ");
			if(!from_string<int>(this->cpunumber,data,std::dec))
			{
				LOGGER(ERROR,"Parsing: CPU entry %s to int",data.c_str());
				throw(-4);

			}
			//CHECK the validity of cpu number

			LOGGER(INFO,"MODULE WILL SET Its Affinity to Specified CPU %d",this->cpunumber);		
		}
		ACM_USER::SET_CPU_AFFINITY(this->cpunumber);
	}


};