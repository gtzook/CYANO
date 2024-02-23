import asyncio
from multiprocessing.synchronize import Event
import time
from masterflexserial.masterflexserial import MasterflexSerial
from typing import Dict, Union

PORT='dev/ttyACM0' # check this

async def enable(port):

    pump = MasterflexSerial(port, "1")
    serial_task = asyncio.create_task(pump.connect())

    async def pump_cmd():
        await asyncio.sleep(1)
        await pump.enable()

    cmd_task = asyncio.create_task(pump_cmd())
    # Run both task at the same time
    await asyncio.gather(serial_task, cmd_task)

async def start(port):

    pump = MasterflexSerial(port, "1")
    serial_task = asyncio.create_task(pump.connect())

    async def pump_cmd():
        await asyncio.sleep(1)
        await pump.start()

    cmd_task = asyncio.create_task(pump_cmd())
    # Run both task at the same time
    await asyncio.gather(serial_task, cmd_task)
    
async def set_speedp(port, value):

    pump = MasterflexSerial(port, "1")
    serial_task = asyncio.create_task(pump.connect())

    async def pump_cmd():
        await asyncio.sleep(1)
        print (await pump.speed_percent(value))

    cmd_task = asyncio.create_task(pump_cmd())
    # Run both task at the same time
    await asyncio.gather(serial_task, cmd_task)
    
def pump_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, Event], 
             debug_mode: bool) -> None:
    asyncio.run(enable(PORT))
    asyncio.run(start(PORT))
    while True:
        time.sleep(1)