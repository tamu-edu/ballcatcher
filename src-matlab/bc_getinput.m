function InputCalculated=bc_getinput(agent, ball, number_input_nodes, params)
    InputCalculated=zeros(1,number_input_nodes);
    
    for sensor_id=1:number_input_nodes
        fprintf('sensor id=%d\n',sensor_id);
        fprintf('ball position=(%d,%d)\n',ball.x_pos,ball.y_pos);
        fprintf('agent position=(%d,0)\n',agent.x_pos);
        sensor_interval=params.sensor_width/(number_input_nodes-1);
        theta=pi*(90+params.sensor_width/2-(number_input_nodes-sensor_id)*sensor_interval)/180;
        %ball_x=ballpair(current_ballpairs).left.x_pos;
        %ball_y=ballpair(current_ballpairs).left.y_pos;
        if sensor_id~=3
        	coeff_a=1+tan(theta)^2;
            coeff_b=-2*(ball.x_pos+tan(theta)*(tan(theta)*agent.x_pos+ball.y_pos));
            coeff_c=ball.x_pos^2+(tan(theta)*agent.x_pos+ball.y_pos)^2-params.ball_radius^2;
            root_formula=coeff_b^2-4*coeff_a*coeff_c;
                   
            if root_formula<0
                fprintf('does not meet\n');
                InputCalculated(sensor_id)=params.sensor_length;
            elseif root_formula==0
                fprintf('touches\n');
                intersect_x=-coeff_b/(2*coeff_a);
                intersect_y=tan(theta)*(intersect_x-agent.x_pos);
                InputCalculated(sensor_id)=sqrt( (agent.x_pos-intersect_x)^2+intersect_y^2 );
            else
                fprintf('meet\n');
                intersect_x=(-coeff_b+sqrt(root_formula))/(2*coeff_a);
                intersect_y=tan(theta)*(intersect_x-agent.x_pos);
                tmp1=sqrt( (agent.x_pos-intersect_x)^2+intersect_y^2 );
                intersect_x=(-coeff_b-sqrt(root_formula))/(2*coeff_a);
                intersect_y=tan(theta)*(intersect_x-agent.x_pos);
                tmp2=sqrt( (agent.x_pos-intersect_x)^2+intersect_y^2 );
                if tmp1<tmp2
                    InputCalculated(sensor_id)=tmp1;
                else
                    InputCalculated(sensor_id)=tmp2;
                end
            end
            fprintf('1. calc dist=%f\n',InputCalculated);
        else
            coeff_a=1;
            coeff_b=-2*ball.y_pos;
            coeff_c=ball.y_pos^2+(agent.x_pos-ball.x_pos)^2-params.ball_radius^2;
            root_formula=coeff_b^2-4*coeff_a*coeff_c;
                   
            if root_formula<0
                InputCalculated(sensor_id)=params.sensor_length;
            elseif root_formula==0
                if -coeff_b/(2*coeff_a)>params.sensor_length
                    InputCalculated(sensor_id)=params.sensor_length;
                else
                    InputCalculated(sensor_id)=-coeff_b/(2*coeff_a);
                end
            else
                if (-coeff_b-sqrt(root_formula))/(2*coeff_a)>params.sensor_length
                    InputCalculated(sensor_id)=params.sensor_length;
                else
                    InputCalculated(sensor_id)=(-coeff_b-sqrt(root_formula))/(2*coeff_a);
                end
            end
            fprintf('2. calc dist=%f\n',InputCalculated);
        end
        pause;
    end