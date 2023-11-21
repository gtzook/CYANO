#!/usr/bin/env python
import PySimpleGUI as sg
from random import randint
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, FigureCanvasAgg
from matplotlib.figure import Figure
from collections import deque
import util.time_formatting
# GUI built from demo: https://github.com/PySimpleGUI/PySimpleGUI/blob/master/DemoPrograms/Demo_Matplotlib_Animated.py

# Method for drawing matplotlib figure on PySimpleGUI Canvas
def draw_figure(canvas, figure, loc=(0, 0)):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg

def gui_loop(adc_data, new_ph_event, light_data):
    ph_datapoints = 5
    
    sg.theme('Dark Green 5') # set gui colors
    
    # pH section
    ph_column = [[sg.Text('pH Sensor', size=(40, 1),
                justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 60), key='-CANVAS-')]]
    
    # light timer
    light_column = [
            [sg.Text('DAY', size=(40, 3), key='-DAY-NIGHT-',
                justification='center', font='Helvetica 40',
                text_color='yellow')],
              
              [sg.Text('Time to switch: ', size=(40, 1),
                justification='center', font='Helvetica 20',
                text_color='gray'), # these elements will be next to each other
              sg.Text('', size=(40, 1), key='-TIME-SWITCH-',
                justification='center', font='Helvetica 20')]
            ]
    
    # define the form layout
    layout = [
            [sg.Column(ph_column), 
              sg.VSeperator(),
              sg.Column(light_column),]
    ]

    # create the form and show it without the plot
    window = sg.Window('CYANO GUI',
                layout, finalize=True, element_justification='c')
    window.maximize()

    canvas_elem = window['-CANVAS-']
    canvas = canvas_elem.TKCanvas

    # draw the initial plot in the window
    fig = Figure()
    fig.patch.set_facecolor('xkcd:seafoam') # background color of plt
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
    
    fig_agg = draw_figure(canvas, fig)

    # deque for ph data
    phs = deque([0]*ph_datapoints,maxlen=ph_datapoints)
    
    # make plot
    line, =ax.plot(range(ph_datapoints), phs, 
                   color='purple', linewidth=6)
    
    while True:
        event, values = window.read(timeout=10)
        if event in ('Exit', None):
            exit(0)
        new_ph_event.wait() # TODO: Plot updating should be in a separate thread
        new_ph_event.clear() # Reset event flag because we are addressing it
        phs.append(adc_data['ph'])  # add new ph data
        line.set_ydata(phs)     # update plot
        fig_agg.draw()
        
        # TODO: below should also be separate thread
        if light_data['state']:
            window['-DAY-NIGHT-'].update(value='NIGHT',
                                         text_color='blue')
        else:
            window['-DAY-NIGHT-'].update(value='DAY',
                                         text_color='yellow')
        
        time_str = util.time_formatting.time_string_from_sec(light_data['remaining'])
        print(light_data['remaining'])
        window['-TIME-SWITCH-'].update(time_str)