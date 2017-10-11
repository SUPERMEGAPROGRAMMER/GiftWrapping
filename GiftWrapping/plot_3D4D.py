import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


def onKeyEvent(event):
    angle_step = .1    
    
    if getattr(event, 'key') == 'q' or getattr(event, 'key') == 'z':
		if getattr(event, 'key') == 'q':
			phi =  angle_step
		else:
			phi = -angle_step
        
		rotation_matrix = np.array([[np.cos(phi), 0., 0., -np.sin(phi)], 
									[		  0., 1., 0., 			0.],
									[		  0., 0., 1., 			0.],
									[np.sin(phi), 0., 0., np.cos(phi)]])

    elif getattr(event, 'key') == 'w' or getattr(event, 'key') == 'x':
		if getattr(event, 'key') == 'w':
			phi =  angle_step
		else:
			phi = -angle_step
		
		rotation_matrix = np.array([[1., 		  0., 0., 			0.],
									[0., np.cos(phi), 0., -np.sin(phi)], 
									[0., 		  0., 1., 			0.],
									[0., np.sin(phi), 0., np.cos(phi)]])
		
    elif getattr(event, 'key') == 'e' or getattr(event, 'key') == 'c':
		if getattr(event, 'key') == 'e':
			phi =  angle_step
		else:
			phi = -angle_step
		
		rotation_matrix = np.array([[1., 0., 		  0., 			0.],
									[0., 1., 		  0., 			0.],
									[0., 0., np.cos(phi), -np.sin(phi)], 
									[0., 0., np.sin(phi), np.cos(phi)]])

    else:
		rotation_matrix = np.array([[1., 0., 0., 0.],
									[0., 1., 0., 0.],
									[0., 0., 1., 0.], 
									[0., 0., 0., 1.]])

    for i in xrange(len(points)):
	    points[i] = rotation_matrix.dot(points[i])  

    projected_points = np.array(map(lambda p: (p / (p[3]-3.))[0:3], points))

    ax.clear() 
    ax.set_xlim(-1., 1.)
    ax.set_ylim(-1., 1.)
    ax.set_zlim(-1., 1.)

	# ~~~~~
    """
    for i in xrange(len(projected_points)):
        ax.text(projected_points[i,0], projected_points[i,1], projected_points[i,2], i, None)
	"""
    # ~~~~~
       
    poly_verts = np.array([projected_points[map(int, faces[counter])] for counter in xrange(len(faces))])
    poly = Poly3DCollection(poly_verts, facecolor=[0., 1., 0.], edgecolor=[0., 0., 0.], alpha=0.02)

    ax.scatter(*zip(*projected_points))
    ax.add_collection3d(poly)
    fig.canvas.draw()


def read_file(file_name):
    res = []
    f = open(file_name, 'r')
    for line in f:
        lst = map(float, line.split())
        res.append(lst)
    f.close()
    return np.array(res)

	
points = read_file('data/points.txt')
faces  = read_file('data/faces.txt')

# -----
# Square
"""
points = np.array([[-1.0, -1.0],
				   [-1.0,  1.0],
				   [ 1.0,  1.0],
                   [ 1.0, -1.0]])

faces = np.array([[0, 1],
                  [1, 2],
                  [2, 3],
                  [3, 0]])
"""
# -----

# -----
is_2D = (len(points[0]) == 2)
# -----

is_4D = (len(points[0]) == 4)

# -----
if is_2D:
    poly_verts = np.array([points[map(int, faces[counter])] for counter in xrange(len(faces))])
# -----
elif is_4D:
    # $$$$$
    faces = np.array([[face[index] for index in xrange(4) if index != index_skip] for face in faces for index_skip in xrange(4)])
    # $$$$$

    projected_points = np.array(map(lambda p: (p / (p[3]-3.))[0:3], points))
    poly_verts = np.array([projected_points[map(int, faces[counter])] for counter in xrange(len(faces))])
    poly = Poly3DCollection(poly_verts, facecolor=[0., 1., 0.], edgecolor=[0., 0., 0.], alpha=0.02)
else:
    poly_verts = np.array([points[map(int, faces[counter])] for counter in xrange(len(faces))])
    poly = Poly3DCollection(poly_verts, facecolor=[0., 0., 1.], edgecolor=[0., 0., 0.], alpha=0.3)

# -----
if is_2D:
    plt.figure()
    for edge in poly_verts:
        plt.plot(*zip(*edge), c='b')
    plt.scatter(*zip(*points), c=[65./255, 105./255, 225./255], s=15, zorder=2)
    
#    for i in xrange(len(points)):
#        plt.text(points[i,0], points[i,1], i, None)
else:
# -----
    fig = plt.figure()
    ax = Axes3D(fig)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    ax.set_xlim(-1., 1.)
    ax.set_ylim(-1., 1.)
    ax.set_zlim(-1., 1.)

    if is_4D:
        ax.scatter(*zip(*projected_points))
    else:
        ax.scatter(*zip(*points))

    ax.add_collection3d(poly)

    # ~~~~~
    """
    if is_4D:
        for i in xrange(len(projected_points)):
            ax.text(projected_points[i,0], projected_points[i,1], projected_points[i,2], i, None)	
    else:	
        for i in xrange(len(points)):
            ax.text(points[i,0], points[i,1], points[i,2], i, None)
    """
    # ~~~~~

    if is_4D:
        key_press_event_id = fig.canvas.mpl_connect('key_press_event', onKeyEvent)
        key_release_event_id = fig.canvas.mpl_connect('key_release_event', onKeyEvent)

plt.show()

if is_4D:
    fig.canvas.mpl_disconnect(key_press_event_id)
    fig.canvas.mpl_disconnect(key_release_event_id)
