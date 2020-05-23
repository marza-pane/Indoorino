
from indoorino.devices import SensorDHT22
from gui.application import ApplicationWindowTemplate
from gui.widgets.devices import SensorDHT22_Widget

if __name__ == '__main__':

    sensor = SensorDHT22('TESTBOARD', 'TESTSENSOR', 8)
    app = ApplicationWindowTemplate()
    sensor_widget = SensorDHT22_Widget(app.win, sensor)
    sensor_widget.configure(bg='blue')
    sensor_widget.place(x=10,y=10,width=500, heigh=500)
    sensor_widget.on_resize()
    app.show()