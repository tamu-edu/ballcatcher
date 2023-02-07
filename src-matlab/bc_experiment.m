%%%%%%%%%%%%%%%% XOR experiment file  (contains experiment, receives genom, decodes it, evaluates it and returns raw fitnesses) (function)

%% Neuro_Evolution_of_Augmenting_Topologies - NEAT 
%% developed by Kenneth Stanley (kstanley@cs.utexas.edu) & Risto Miikkulainen (risto@cs.utexas.edu)
%% Coding by Christian Mayr (matlab_neat@web.de)

function [population_plus_fitnesses,catches,misses]=bc_experiment(population,number_input_nodes,number_hidden_nodes,params);
population_plus_fitnesses=population;
no_change_threshold=5; %threshold to judge if state of a node has changed significantly since last iteration
number_individuals=size(population,2);
            
catches=[];
misses=[];
%%%%%
% initialize ballpairs
margin=20;
max_distance=params.sensor_length*sin(1.0/9.0*pi)-10;
default_y=params.sensor_length+params.ball_radius;


%%%%%
for index_individual=1:number_individuals
    
    for pairID=1:params.number_ballpairs
        ballpair(pairID).left.x_pos=-(max_distance-margin*rand);
        ballpair(pairID).left.y_pos=default_y;
        ballpair(pairID).left.caught=0;
        ballpair(pairID).left.fallen=0;
        ballpair(pairID).left.checked=0;
        ballpair(pairID).right.x_pos=max_distance-margin*rand;
        ballpair(pairID).right.y_pos=default_y;
        ballpair(pairID).right.caught=0;
        ballpair(pairID).right.fallen=0;
        ballpair(pairID).right.checked=0;
        if mod(pairID,2)==1     % left faster
            ballpair(pairID).left.speed=2;
            ballpair(pairID).right.speed=1;
        else                    % right faster
            ballpair(pairID).left.speed=1;
            ballpair(pairID).right.speed=2;
        end
        %ballpair(pairID).left
        %ballpair(pairID).right
        %%%%%
       %pause;
    end
    
    index_individual
    % initialize ball catcher agent
    agent.x_pos=0;
    agent.fitness=0;
    agent.speed=1;
    %agent.pos_difference=0;
    agent.catch=0;
    agent.miss=0;
    
    number_nodes=size(population(index_individual).nodegenes,2);
    number_connections=size(population(index_individual).connectiongenes,2);
    individual_fitness=0;
    output1=[];
    output2=[];
    count_ballpairs=0;
    task_clock=0;
   
    %%%%%
    agent_motions=[];
    for current_ballpairs=1:params.number_ballpairs
        current_ballpairs
        ballpair(current_ballpairs).left
        ballpair(current_ballpairs).right
        cur_agent_motion=[];
        agent.x_pos=0;
       %while ballpair(params.number_ballpairs).left.fallen==0 || ballpair(params.number_ballpairs).right.fallen==0    % loop until the last.left and last.right balls are both fallen
       while ballpair(current_ballpairs).left.fallen==0 || ballpair(current_ballpairs).right.fallen==0    % loop until the last.left and last.right balls are both fallen
           % clock ticking
           task_clock=task_clock+1;
           
           if ballpair(current_ballpairs).left.fallen==0                                % IF left ball not yet marked as fallen
               if ballpair(current_ballpairs).left.y_pos<0                              % IF y_pos just now becomes below 0 (ball just landed)
                   ballpair(current_ballpairs).left.fallen=1;                           % mark as fallen
                   distance=abs(agent.x_pos-ballpair(current_ballpairs).left.x_pos);    % calculate x-distance between agent and the fallen ball
                   agent.fitness=agent.fitness+params.environment_width-distance;              % update fitness
                   agent.miss=agent.miss+1;                                             % miss++
               else                                                                     % IF y_pos still above 0 (ball in the air)
                   distance3d=sqrt( (agent.x_pos-ballpair(current_ballpairs).left.x_pos)^2+ballpair(current_ballpairs).left.y_pos^2 );
                                                                                        % calculate distance between agent and the ball in the air
                   if distance3d<(params.agent_radius+params.ball_radius)                             % If agent meets the ball
                       distance=abs(agent.x_pos-ballpair(current_ballpairs).left.x_pos);% calculate x-distance between agent and the ball in the air
                       agent.fitness=agent.fitness+params.environment_width-distance;          % update fitness
                       agent.catch=agent.catch+1;                                       % catch++
                       ballpair(current_ballpairs).left.caught=1;                       % mark as caught
                       ballpair(current_ballpairs).left.fallen=1;                       % mark as fallen
                   end
               end
           end
           if ballpair(current_ballpairs).right.fallen==0                               % do exactly same for the right ball
               if ballpair(current_ballpairs).right.y_pos<0
                   ballpair(current_ballpairs).right.fallen=1;
                   distance=abs(agent.x_pos-ballpair(current_ballpairs).right.x_pos);
                   agent.fitness=agent.fitness+params.environment_width-distance;
                   agent.miss=agent.miss+1;
               else
                   distance3d=sqrt( (agent.x_pos-ballpair(current_ballpairs).right.x_pos)^2+ballpair(current_ballpairs).right.y_pos^2 );
                   if distance3d<(params.agent_radius+params.ball_radius)
                       distance=abs(agent.x_pos-ballpair(current_ballpairs).right.x_pos);
                       agent.fitness=agent.fitness+params.environment_width-distance;
                       agent.catch=agent.catch+1;
                       ballpair(current_ballpairs).right.caught=1;
                       ballpair(current_ballpairs).right.fallen=1;
                   end
               end
           end
           %pause;
           % calculate agent input
           input_leftball=bc_getinput(agent,ballpair(current_ballpairs).left,number_input_nodes,params);
           input_rightball=bc_getinput(agent,ballpair(current_ballpairs).right,number_input_nodes,params);
           
           sensor_input=zeros(1,number_input_nodes);
           network_input=zeros(1,number_input_nodes);
           
           %if index_individual>1
               %sensor_input
               %network_input
               %pause;
           %end
           
           for input_id=1:number_input_nodes
               sensor_input(input_id)=min(input_leftball(1,input_id),input_rightball(1,input_id));
               network_input(input_id)=(params.sensor_length-sensor_input(input_id))/params.sensor_length;
           end
           
           % update agent location
           population(index_individual).nodegenes(3,1:number_input_nodes)=network_input;
           population(index_individual).nodegenes(4,1:number_input_nodes)=population(index_individual).nodegenes(3,1:number_input_nodes);
           no_change_count=0;
           index_loop=0;
           while(no_change_count<number_nodes) && (index_loop<3*number_connections)
               index_loop=index_loop+1;
               vector_node_state=population(index_individual).nodegenes(4,:);
               
               for index_connections=1:number_connections
                   ID_connection_from_node=population(index_individual).connectiongenes(2,index_connections);
                   ID_connection_to_node=population(index_individual).connectiongenes(3,index_connections);
                   connection_weight=population(index_individual).connectiongenes(4,index_connections);
                   
                   index_connection_from_node=find((population(index_individual).nodegenes(1,:)==ID_connection_from_node));
                   index_connection_to_node=find((population(index_individual).nodegenes(1,:)==ID_connection_to_node));
                   
                   if population(index_individual).connectiongenes(5,index_connections)==1      % if this connection is enabled
                       population(index_individual).nodegenes(3,index_connection_to_node)=population(index_individual).nodegenes(3,index_connection_to_node)+population(index_individual).nodegenes(4,index_connection_from_node)*connection_weight;
                   end
               end
               
               population(index_individual).nodegenes(4,number_input_nodes+1:number_nodes)=tanh(population(index_individual).nodegenes(3,number_input_nodes+1:number_nodes));
               population(index_individual).nodegenes(3,number_input_nodes+1:number_nodes)=0;
               
               no_change_count=sum(abs(population(index_individual).nodegenes(4,:)-vector_node_state)<no_change_threshold);
           end
           
           output1=[output1;population(index_individual).nodegenes(4,number_input_nodes+number_hidden_nodes+1)];
           output2=[output2;population(index_individual).nodegenes(4,number_input_nodes+number_hidden_nodes+2)];
           
           positional_offset=output1(length(output1))-output2(length(output2));
           
           if positional_offset<0
               agent.x_pos=agent.x_pos+params.agent_speed;
           else
               agent.x_pos=agent.x_pos-params.agent_speed;
           end
           
           %%%%%
           cur_agent_motion=[cur_agent_motion, agent.x_pos];
           
           % boundary check
           if agent.x_pos>params.environment_width/2-params.agent_radius
               agent.x_pos=params.environment_width/2-params.agent_radius;
           end
           if agent.x_pos<(-params.environment_width/2+params.agent_radius)
               agent.x_pos=-params.environment_width/2+params.agent_radius;
           end
           
           % drop balls
           if ballpair(current_ballpairs).left.fallen==0
                ballpair(current_ballpairs).left.y_pos=ballpair(current_ballpairs).left.y_pos-ballpair(current_ballpairs).left.speed;
           end
           if ballpair(current_ballpairs).right.fallen==0
               ballpair(current_ballpairs).right.y_pos=ballpair(current_ballpairs).right.y_pos-ballpair(current_ballpairs).right.speed;
           end
       end
       
       %%%%%
       %ballpair(current_ballpairs).left.y_pos
       %ballpair(current_ballpairs).left.fallen
       %ballpair(current_ballpairs).right.y_pos
       %ballpair(current_ballpairs).right.fallen
       %agent.catch
       %agent.miss
       plot(cur_agent_motion);
       
       %cur_agent_motion
       pause;
    end
       
    population_plus_fitnesses(index_individual).fitness=agent.fitness;
%    population_plus_fitnesses(index_individual).catch=agent.catch;
%    population_plus_fitnesses(index_individual).miss=agent.miss;
    index_individual
    catches=[catches,agent.catch];
    misses=[misses,agent.miss];
    agent.catch
    agent.miss
    population_plus_fitnesses(index_individual).nodegenes
    population_plus_fitnesses(index_individual).connectiongenes
    pause;
    
    if agent.catch==2*params.number_ballpairs
        population_plus_fitnesses(index_individual).neurongenes
        population_plus_fitnesses(index_individual).connectiongenes
    end
    %%%%%
    %population_plus_fitnesses(index_individual).fitness=(4-individual_fitness)^2; %Fitness function as defined by Kenneth Stanley    
    %if sum(abs(round(output)-output_pattern))==0      
    %    population_plus_fitnesses(index_individual).fitness=16;
    %    output
    %end
end
