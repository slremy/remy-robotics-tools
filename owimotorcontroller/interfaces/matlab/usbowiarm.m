%Code inspired by example of the playermex project and matlabcentral posts by O. Woodford (8 Jun, 2010)
classdef usbowiarm < handle
    properties (Hidden = true, SetAccess = private)
        cpp_handle;
    end
    methods
        % Constructor
        function this = usbowiarm(armid)
            this.cpp_handle = owiswig_m('create_arm',armid);
        end
        % Destructor
        function delete(this)
            owiswig_m('destroy_arm', this.cpp_handle);
        end
        % Action methods
        function halt_motors(this)
            owiswig_m('halt_motors',this.cpp_handle);
        end
        function set_control(this)
            owiswig_m('set_control',this.cpp_handle);
        end
        function output = get_control(this)
            output = owiswig_m('get_control',this.cpp_handle);
        end
        function setup_LEDON(this)
            owiswig_m('setup_LEDON',this.cpp_handle);
        end
        function setup_LEDOFF(this)
            owiswig_m('setup_LEDOFF',this.cpp_handle);
        end
        function setup_LEDTOGGLE(this)
            owiswig_m('setup_LEDTOGGLE',this.cpp_handle);
        end
        function setup_motorforward(this)
            owiswig_m('setup_motorforward',this.cpp_handle);
        end
        function setup_motorreverse(this)
            owiswig_m('setup_motorreverse',this.cpp_handle);
        end
        function setup_motoroff(this)
            owiswig_m('setup_motoroff',this.cpp_handle);
        end
        function test(this)
             owiswig_m('test',this.cpp_handle);
        end
    end
end

