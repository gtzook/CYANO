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
    figure_canvas_agg.get_tk_widget().pack(side='bottom', fill='both', expand=1)
    return figure_canvas_agg

def gui_loop(adc_data, new_ph_event, light_data):
    ph_datapoints = 5
    od_datapoints = 30
    
    sg.theme('Dark') # set gui colors
    
    # pH section
    ph_column = [[sg.Text('pH Sensor', size=(40, 1), key='-PH-',
                justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 60), key='-CANVAS-')]]
    
    od_column = [[sg.Text('OD Sensor', size=(40, 1), key='-OD-',
                justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 60), key='-OD-CANVAS-')]]
    
    status_column = [
    [sg.Text('pH Value:', font='Helvetica 32'), sg.Text('', key='-PH-VALUE-', font='Helvetica 32')],
    [sg.Text('', font='Helvetica 32'), sg.Text('', key="-BLANK-", font = 'Helvetica 32')],
    [sg.Text('OD Value:', font='Helvetica 32'), sg.Text('', key='-OD-VALUE-', font='Helvetica 32')]
]
    
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
    spacer_column = [sg.Column([[]], size=(140, 1))]
    s_c2 = [sg.Column([[]], size=(140, 1))]
    s_light = [sg.Column([[]], size=(325, 1))]
    top = [sg.Column([[]], size=(250, 200))]
    layout = [[*top],
            [*s_light, sg.Column(light_column, size = (3500, 350), element_justification = 'center'),],
              [sg.Column(ph_column,vertical_alignment ='bottom'), *spacer_column, sg.Column(status_column), *s_c2, sg.Column(od_column, vertical_alignment = 'bottom')]
    ]

    # create the form and show it without the plot
    window = sg.Window('CYANO GUI',
                layout, finalize=True)
    window.maximize()

    canvas_elem = window['-CANVAS-']
    canvas = canvas_elem.TKCanvas

    #PH PLOT

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
    
    
    # make ph plot
    ph_line, =ax.plot(range(ph_datapoints), phs, 
                   color='purple', linewidth=6)    

    #OD PLOT
    fig2 = Figure()
    fig2.patch.set_facecolor('xkcd:seafoam') # background color of plt
    ax2 = fig2.add_subplot(111)
    ax2.set_xlabel("Time")
    ax2.set_ylabel("OD")
    ax2.grid()
    ax2.set_ylim(0,5000)
    for item in ([ax2.xaxis.label, ax2.yaxis.label] +
             ax2.get_xticklabels() + ax2.get_yticklabels()):
        item.set_fontsize(20) # make fonts bigger
    for item in (ax2.get_xticklabels() + ax2.get_yticklabels()):
        item.set_fontsize(15) # make fonts bigger
    fig2.tight_layout() #tight layout for aesthetics
    fig2.subplots_adjust(bottom=0.15,left=0.12) #expand to include text
    
    od_canvas_elem = window['-OD-CANVAS-']
    od_canvas = od_canvas_elem.TKCanvas
    fig_agg2 = draw_figure(od_canvas, fig2)

    # deque of od data
    ods = deque([0]*od_datapoints,maxlen=od_datapoints)
    
    # make od plot
    od_line, =ax2.plot(range(od_datapoints), ods, 
                   color='green', linewidth=6)

    while True:
        event, values = window.read(timeout=10)
        if event in ('Exit', None):
            exit(0)
        new_ph_event.wait() # TODO: Plot updating should be in a separate thread
        new_ph_event.clear() # Reset event flag because we are addressing it
        phs.append(adc_data['ph'])  # add new ph data
        ph_line.set_ydata(phs)     # update plot
        
        ods.append(adc_data['od']) # add new od data
        od_line.set_ydata(ods)     # update plot
        
        fig_agg.draw()
        fig_agg2.draw()
        
        # TODO: below should also be separate thread
        window['-PH-VALUE-'].update(value="{:.3f}".format(adc_data['ph']))
        window['-OD-VALUE-'].update(value="{:.3f}".format(adc_data['od']))

        if light_data['state']:
            window['-DAY-NIGHT-'].update(value='NIGHT',
                                         text_color='blue')
        else:
            window['-DAY-NIGHT-'].update(value='DAY',
                                         text_color='yellow')
        
        time_str = util.time_formatting.time_string_from_sec(light_data['remaining'])
        window['-TIME-SWITCH-'].update(time_str)