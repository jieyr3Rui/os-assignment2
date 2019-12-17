import numpy as np
import copy
import os
import argparse
import matplotlib.pyplot as plt
import re
os.chdir(os.path.split(os.path.realpath(__file__))[0])

# global variable
num_process = 6
max_run_process_num = 3
num_page = 256
delta = 100
x_delta = []
y_page = []
run_time = 0
address_file = open('addresses-locality.txt', 'w')


def reset_global_value(p, r, d):
    global num_process, max_run_process_num, delta, run_time, x_delta, y_page, address_file
    num_process = p if p < 32 and p > 0 else num_process
    max_run_process_num = r if r <= num_process and r > 0 else max_run_process_num
    delta = d if d > 10 and d < 1000 else delta
    run_time = 0
    x_delta.clear()
    y_page.clear()
    address_file.close()
    address_file = open('addresses-locality.txt', 'w')
    return True

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
    
    
def strat_a_running():
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

    plt.figure(figsize=[15, 10])
    plt.scatter(x_delta, y_page)
    global size_of_physics
    plt.title('proc_num_' + str(num_process) + '_run_' + str(max_run_process_num) + '_physics_' + str(size_of_physics) +'_delta_' + str(delta) + '_page')
    plt.xlabel('run_time/delta')
    plt.ylabel('page')
    path = 'traces/proc_num_' + str(num_process) + '_run_' + str(max_run_process_num) + '_physics_' + str(size_of_physics) + '_delta_' + str(delta)+ '_page.png'
    plt.savefig(path)
    print('save image: ' + path)

if __name__ == '__main__':
    path = 'traces/'
    for filename in os.listdir(path):
        os.remove(path + filename)
    
    pnum = []
    lru_page_fault = []
    lru_tlb_hit = []
    fifo_page_fault = []
    fifo_tlb_hit = []
    global size_of_physics
    size_of_physics = 256
    for ii in range(4):
        pnum.append(ii)
        reset_global_value(6, 3, 100)
        print('strat generating addresses...')
        strat_a_running()
        print('finish generating addresses')

        print('start runnign vm using lru...')
        os.system('./vm BACKING_STORE.bin addresses.txt -aaddresses-locality.txt -blru -c' + str(int(size_of_physics)) + ' -drate > out-locality.txt')
        print('finish running vm')
        with open('out-locality.txt', 'r') as local:
            line = local.readline()
            [tlb_hit] = re.findall(r'\d+\.?\d*', line)
            lru_tlb_hit.append(tlb_hit)
            line = local.readline()
            [page_fault] = re.findall(r'\d+\.?\d*', line)
            print(page_fault)
            lru_page_fault.append(page_fault)
        
        print('start runnign vm using fifo...')
        os.system('./vm BACKING_STORE.bin addresses.txt -aaddresses-locality.txt -bfifo -c' + str(int(size_of_physics)) + ' -drate > out-locality.txt')
        print('finish running vm')
        with open('out-locality.txt', 'r') as local:
            line = local.readline()
            [fifo_hit] = re.findall(r'\d+\.?\d*', line)
            fifo_tlb_hit.append(tlb_hit)
            line = local.readline()
            [page_fault] = re.findall(r'\d+\.?\d*', line)
            fifo_page_fault.append(page_fault)
            print(page_fault)
        size_of_physics = size_of_physics / 2

    print('start plotting')
    plt.figure(figsize=[8, 6])
    plt.plot(pnum, lru_page_fault, c='b')
    plt.plot(pnum, fifo_page_fault, c='r')
    plt.xlabel('num of process')
    plt.ylabel('page fault rate')
    plt.savefig('page_fault.png')

    plt.figure(figsize=[8, 6])
    plt.plot(pnum, lru_tlb_hit, c='b')
    plt.plot(pnum, fifo_tlb_hit, c='r')
    plt.xlabel('num of process')
    plt.ylabel('tlb hit rate')
    plt.savefig('tlb_hit.png')
    print('finish plotting')
    # reset_global_value(6, 3, 100)
    # print('strat generating addresses...')
    # strat_a_running()
    # print('finish generating addresses')
    # print('start runnign vm...')
    # os.system('./vm BACKING_STORE.bin addresses.txt -aaddresses-locality.txt -blru -c128 -drate ')
    # print('finish running vm')

