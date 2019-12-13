import numpy as np
import copy
import os
import matplotlib.pyplot as plt
os.chdir(os.path.split(os.path.realpath(__file__))[0])

# global variable
num_process = 4
max_run_process_num = 3
num_page = 256
delta = 100
x_delta = []
y_page = []
run_time = 0
address_file = open('addresses-locality.txt', 'w')

class proccess():
    def __init__(self, pid, pages):
        self.pid = pid
        self.pages = pages
        self.num_pages = len(pages)
        self.state = 'sleep' # run wait sleep
        self.time_tick = 0
        self.priority = np.random.randint(0, num_process)

    # run once
    def run(self):
        if self.state != 'run' or self.time_tick == 0:
            return False
        global run_time
        if run_time >= 10000:
            return False
        self.time_tick = self.time_tick - 1

        page = self.pages[np.random.randint(0,self.num_pages)]
        offset = np.random.randint(0, 256)
        y_page.append(page)
        
        x_delta.append(int(run_time/delta))
        run_time += 1
        address_file.write(str(page*256+offset)+'\n')
        return True

    # sleep once
    def sleep(self):
        if self.state != 'sleep' or self.time_tick == 0:
            return False

        self.time_tick = self.time_tick - 1
        
        return True
    

    def set_state(self, state):
        # cannot jump
        if self.state == 'sleep' and state == 'run':
           return False
        # reset time_tick by random function
        if (self.state == 'sleep' and state == 'wait') or \
           (self.state == 'run'  and  state == 'sleep'):
           self.time_tick = np.random.randint(1, 1000)
        self.state = state
        return True

    def get_state(self):
        return self.state

    def jump(self):
        if self.time_tick == 0:
            return True
        else:
            return False

def ensure_each_element_positive(arr):
    add = 0
    num = arr.size
    for ii in range(num):
        if arr[ii] == 0:
            arr[ii] += 1
            add += 1
    while add > 0:
        for ii in range(num):
            if arr[ii] > 1:
                arr[ii] -= 1
                add -= 1
                break
    return arr

# p1.priority = 3 p2.priority = 4 
# ==> p1 higher than p2
# ==> sort_list [p1, p2]
def sort_list(plist):
    if plist == [] or plist == None:
        return []
    def get_priority(proccess):
        return proccess.priority
    def get_pid(proccess):
        return proccess.pid

    plist.sort(key=get_priority, reverse=False)
    list_prior = []
    list_prior.append(plist[0].priority)
    for p in plist:
        if list_prior[len(list_prior)-1] != p.priority:
            list_prior.append(p.priority)
    result = []
    for prior in list_prior:
        prior_plist = []
        for p in plist:
            if p.priority == prior:
                prior_plist.append(p)
        prior_plist.sort(key=get_pid, reverse=False)
        result.extend(prior_plist.copy())
    return result.copy()



def print_state(plist_sleep, plist_wait, plist_run):
    out = 'r: '
    for p in plist_run:
        out += '['+ str(p.pid) + ' ' + str(p.priority) + ' %3d'%p.time_tick + '] '
    out += 'w: '
    for p in plist_wait:
        out += '['+ str(p.pid) + ' ' + str(p.priority) + ' %3d'%p.time_tick + '] '
    out += 's: '
    for p in plist_sleep:
        out += '['+ str(p.pid) + ' ' + str(p.priority) + ' %3d'%p.time_tick + '] '
    print(out)

def running(plist_sleep, plist_wait, plist_run):
    print('strat running')
    while run_time < 10000:
        # run to sleep
        new_plist_run = []
        for p in plist_run:
            if p.jump():
                p.set_state('sleep')
                plist_sleep.append(p)
            else:
                new_plist_run.append(p)
        plist_run = new_plist_run.copy()
        new_plist_run.clear()

        # sleep to wait
        new_plist_sleep = []
        for p in plist_sleep:
            if p.jump():
                p.set_state('wait')
                plist_wait.append(p)
            else:
                new_plist_sleep.append(p)
        plist_sleep = new_plist_sleep.copy()
        new_plist_sleep.clear()

        # wait to run and run to wait
        # merge wait and run
        plist_merge = sort_list(plist_wait.copy() + plist_run.copy())
        plist_run.clear()
        plist_wait.clear()
        num_merge = len(plist_merge)
        if num_merge >= max_run_process_num:
            for ii in range(max_run_process_num):
                plist_run.append(plist_merge.pop(0))
            plist_wait = plist_merge.copy()
        else:
            plist_run = plist_merge.copy()
        plist_merge.clear()
        for p in plist_wait:
            p.set_state('wait')
        for p in plist_run:
            p.set_state('run')
        
        for p in plist_sleep:
            p.sleep()
        for p in plist_run:
            p.run()
        # print_state(plist_sleep, plist_wait, plist_run)
    print('finish running')
    
if __name__ == '__main__':
    ramdon_process_page = np.random.rand(num_process)
    num_process_frame = np.array(num_page * ramdon_process_page / np.sum(ramdon_process_page)).astype(int)
    num_process_frame = ensure_each_element_positive(num_process_frame)
    print('num of pages: ', num_process_frame)

    proccesses = []
    index = 0

    pid = 0
    for ii in range(num_process):
        pages = np.arange(index, index + num_process_frame[ii])
        index += num_process_frame[ii]
        proccesses.append(proccess(pid, pages))
        pid = pid + 1


    processes_sleep = proccesses.copy()
    processes_wait  = []
    processes_run   = []

    running(processes_sleep, processes_wait, processes_run)

    plt.scatter(x_delta, y_page)
    plt.show()