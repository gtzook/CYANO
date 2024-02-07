#!/usr/bin/env python
import PySimpleGUI as sg
from random import randint
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from collections import deque
import util.time_formatting
from typing import Dict, Union
import multiprocessing as mp
import time
import signal
import sys
# GUI built from demo: https://github.com/PySimpleGUI/PySimpleGUI/blob/master/DemoPrograms/Demo_Matplotlib_Animated.py

# Method for drawing matplotlib figure on PySimpleGUI Canvas
def draw_figure(canvas, figure, loc=(0, 0)):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='bottom', fill='both', expand=1)
    return figure_canvas_agg

def gui_loop(shared_data: Dict[str, Union[int,float,bool]], 
             events: Dict[str, mp.Event], 
             debug_mode: bool) -> None:
    ph_datapoints = 30 # max number of ph datapoints to display at once
    od_datapoints = 30 # max number of od datapoints to display at once
    
    #sg.theme('Dark') # set gui colors
    
    # pH section
    ph_column = [[sg.Text('pH Sensor', size=(40, 1), key='-PH-',
                justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 60), key='-PH-CANVAS-')]]
    
    # OD section
    od_column = [[sg.Text('OD Sensor', size=(40, 1), key='-OD-',
                justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 60), key='-OD-CANVAS-')]]
    
    #Value Display section
    status_column = [
    [sg.Text('pH Value:', font='Helvetica 32'), sg.Text('', key='-PH-VALUE-', font='Helvetica 32')],
    [sg.Text('', font='Helvetica 32'), sg.Text('', key="-BLANK-", font = 'Helvetica 32')],
    [sg.Text('OD Value:', font='Helvetica 32'), sg.Text('', key='-OD-VALUE-', font='Helvetica 32')],
    [sg.Button('Blank', font = "Helvetica 16", size = (10,3))]
    ]

    #Agiation Display section
    agiation_column = [
        [sg.Text('Agitation:', font = 'Helvetica 32'), sg.Text('', font = 'Helvetica 32', key='-AGITATION-PERCENT-')],
        [sg.Button('Increase', font = 'Helvetica 16', size = (10,3)), sg.Button('Decrease', font= 'Helvetica 16', size = (10,3))],
    ]

    #Day Duration Slider
    slider_column = [
        [sg.Text("Daytime Hours:", font = "Helvetica 32")],
        [sg.Slider(range=(0, 24), orientation="h", size=(60, 40), default_value=12, key="-SLIDER-")],
        [sg.Button("OK"), sg.Button("Cancel")]
    ]

    
    # light timer
    light_column = [
            [sg.Text('DAY', size=(30, 3), key='-DAY-NIGHT-',
                justification='center', font='Helvetica 40',
                text_color='yellow')],
              
              [sg.Text('Time to switch: ', size=(40, 1),
                justification='center', font='Helvetica 20',
                text_color='gray'), # these elements will be next to each other
              sg.Text('', size=(30, 1), key='-TIME-SWITCH-',
                justification='center', font='Helvetica 20')]
            ]
    
    # define the form layout
    #some of these are used for manipulating the spacing and location of the objects on the screen
    spacer_column = [sg.Column([[]], size=(140, 1))]
    s_c2 = [sg.Column([[]], size=(140, 1))]
    s_light = [sg.Column([[]], size=(325, 1))]
    top = [sg.Column([[]], size=(250, 200))]

    layout = [[*top], [sg.Column(agiation_column), sg.Column(light_column, element_justification = 'center'), sg.Column(slider_column) ],
              [sg.Column(ph_column,vertical_alignment ='bottom'), *spacer_column, sg.Column(status_column), *s_c2, sg.Column(od_column, vertical_alignment = 'bottom')]
    ]
    
    # PH PLOT
    # draw the initial plot in the window
    fig = Figure()
    ax = fig.add_subplot(111)
    ax.set_xlabel("Time")
    ax.set_ylabel("pH")
    ax.grid()
    ax.set_ylim(0,10)
    for item in ([ax.xaxis.label, ax.yaxis.label] +
            ax.get_xticklabels() + ax.get_yticklabels()):
        item.set_fontsize(20) # make fonts bigger
    for item in (ax.get_xticklabels() + ax.get_yticklabels()):
        item.set_fontsize(15) # make fonts bigger
    fig.tight_layout() #tight layout for aesthetics
    fig.subplots_adjust(bottom=0.15,left=0.12) #expand to include text

    # deque for ph data
    phs = deque([0]*ph_datapoints,maxlen=ph_datapoints)
        
    # make ph plot
    ph_line, =ax.plot(range(ph_datapoints), phs, 
                color='purple', linewidth=6)    
    
    #OD PLOT
    fig2 = Figure()
    ax2 = fig2.add_subplot(111)
    ax2.set_xlabel("Time")
    ax2.set_ylabel("OD")
    ax2.grid()
    ax2.set_ylim(0,4000)
    for item in ([ax2.xaxis.label, ax2.yaxis.label] +
            ax2.get_xticklabels() + ax2.get_yticklabels()):
        item.set_fontsize(20) # make fonts bigger
    for item in (ax2.get_xticklabels() + ax2.get_yticklabels()):
        item.set_fontsize(15) # make fonts bigger
    fig2.tight_layout() #tight layout for aesthetics
    fig2.subplots_adjust(bottom=0.15,left=0.2) #expand to include text
    
    # deque of od data
    ods = deque([0]*od_datapoints,maxlen=od_datapoints)
    
    # make od plot
    od_line, =ax2.plot(range(od_datapoints), ods, 
                color='green', linewidth=6)
    # Define your splash screen layout
    splash_layout = [[sg.Text('Loading...')]]
    size = sg.Window.get_screen_size()
    try:       
        # create the form and show it without the plot
        splash = sg.Window('Splash Screen', splash_layout, finalize=True, no_titlebar=True, 
                            grab_anywhere=True, size=size, keep_on_top=True, force_toplevel=True)
        splash.read(timeout=3000, close=True)
        window = sg.Window('CYANO GUI',
                    layout, finalize=True, size=size)
        ph_canvas_elem = window['-PH-CANVAS-']
        ph_canvas = ph_canvas_elem.TKCanvas     
        
        od_canvas_elem = window['-OD-CANVAS-']
        od_canvas = od_canvas_elem.TKCanvas
            
        agitation_percent = 0
        
        window.un_hide()
        print("Unhide")
        fig_agg2 = draw_figure(od_canvas, fig2) 
        fig_agg = draw_figure(ph_canvas, fig)   
        
        def window_update():
            fig_agg.draw() # render plots
            fig_agg2.draw()
            
            window['-PH-VALUE-'].update(value="{:.3f}".format(shared_data['ph'])) # update text displays
            window['-OD-VALUE-'].update(value=f"{shared_data['od']}")
            window['-AGITATION-PERCENT-'].update(f'{agitation_percent}%')

            if shared_data['state']:
                window['-DAY-NIGHT-'].update(value='DAY',
                                            text_color='yellow')
            else:
                window['-DAY-NIGHT-'].update(value='NIGHT',
                                            text_color='blue')
            
            time_str = util.time_formatting.time_string_from_sec(shared_data['remaining'])
            window['-TIME-SWITCH-'].update(time_str)
        time.sleep(3)
        while True:
            event, _ = window.read(timeout=10)
            if event in ('Exit', None):
                break
            elif event == 'Increase':
                agitation_percent  += 10
            elif event == 'Decrease':
                agitation_percent -= 10
                
            phs.append(shared_data['ph'])  # add new ph data
            ph_line.set_ydata(phs)     # update plot
            
            ods.append(shared_data['od']) # add new od data
            od_line.set_ydata(ods)     # update plot
            
            window_update()
            
            time.sleep(0.05) # ~refresh rate of 20 Hz
    except KeyboardInterrupt:
        print('gui: Exiting cleanly')
        window.close()
        sys.exit(0)