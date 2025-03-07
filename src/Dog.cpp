#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "Dog.h"
#include "Log.h"
#include "Common.h"
#include "Tools.h"

int Dog::whistle_msg_id_ = -1;
int Dog::bark_msg_id_ = -1;

void Dog::Run()
{
    CreateWhistleAndBark();

    Whistle whistle;

    while(true)
    {
        if (msgrcv(whistle_msg_id_, &whistle, sizeof(whistle), 1, 0) == -1)
        {
            cape::cout << cape::time << "消息队列接收失败!" << cape::endl;
        }
        else
        {
            cape::cout << cape::time << "[" << whistle.unique_id_ << " " <<
                cape::get_enum_name(whistle.command_type_) << " " << whistle.text << "]" << cape::endl;
        }
    }

    /// 删除消息队列
    if (msgctl(whistle_msg_id_, IPC_RMID, nullptr) == -1)
    {
        cape::cout << "[Dog]: 删除消息队列失败!" << cape::endl;
    }
    cape::cout.Close();
}

void Dog::CreateWhistleAndBark()
{
    key_t key = ftok("whistle", 65);
    whistle_msg_id_ = msgget(key, 0666 | IPC_CREAT);

    if (whistle_msg_id_ == -1)
    {
        cape::cout << cape::time << "消息队列 whistle 创建失败!" << cape::endl;
        kill(-getsid(getpid()), SIGKILL);
    }

    key = ftok("bark", 66);
    bark_msg_id_ = msgget(key, 0666 | IPC_CREAT);

    if (bark_msg_id_ == -1)
    {
        cape::cout << cape::time << "创建消息队列 bark 失败!" << cape::endl;
        kill(-getsid(getpid()), SIGKILL);
    }
}