#include <yed/plugin.h>
#include <stdio.h>



/*COMMANDS */
void git_variables_post_pump(yed_event *event);
/*END COMMANDS*/


static unsigned long long last_update_time;

int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler h;

    YED_PLUG_VERSION_CHECK();

    h.kind = EVENT_POST_PUMP;
    h.fn   = git_variables_post_pump;
    yed_plugin_add_event_handler(self, h);


    yed_set_var("git-variables-branch", "");
    last_update_time=0;

    if(yed_get_var("git-variables-update-interval")==NULL){
        yed_set_var("git-variables-update-interval","1000");
    }
    return 0;
}

void git_variables_post_pump(yed_event *event){
    unsigned long long current_time;
    char *buffer=NULL;
    int status;
    int output_len;
    unsigned long long update_interval;
    if(sscanf(yed_get_var("git-variables-update-interval"),"%llu",&update_interval)!=1){
        update_interval=1000;
    }
    current_time = measure_time_now_ms();
    if((current_time-last_update_time)>update_interval){
        buffer = yed_run_subproc("git rev-parse --abbrev-ref HEAD",&output_len,&status);
        if(status != 0||buffer==NULL){
            yed_set_var("git-variables-branch", "");
        }
        else{
            yed_set_var("git-variables-branch", buffer);
        }
        if(buffer!=NULL){
            free(buffer);
        }
        last_update_time=measure_time_now_ms();
    }
}
