#include "Application.h"
#include "ResourceMesh.h"
#include "Glew/include/glew.h"
#include <gl/GL.h>

ResourceMesh::ResourceMesh(UID uuid) : Resource(uuid, Resource_Mesh)
{
}

ResourceMesh::~ResourceMesh()
{
	RELEASE_ARRAY(index);
	RELEASE_ARRAY(vertex);
	RELEASE_ARRAY(normal);
	RELEASE_ARRAY(uv);
}

bool ResourceMesh::LoadInMemory()
{
	if (!IsPrimitive()) {
		string path = std::to_string(uuid) + "." + MESH_EXTENSION;
		App->mesh_import->LoadMesh(path.c_str(), this);

		//Bind buffers
		BindBuffers();
	}
	else {
		switch (is_primitive) {
		case PRIMITIVE_CUBE: {
			par_shapes_mesh* cube = par_shapes_create_cube();
			LoadMeshPrimitive(cube);
			par_shapes_free_mesh(cube);
		}
			break;
		case PRIMITIVE_SPHERE: {
			par_shapes_mesh* sphere = par_shapes_create_subdivided_sphere(3);
			LoadMeshPrimitive(sphere);
			par_shapes_free_mesh(sphere);
		}
			break;
		case PRIMITIVE_PLANE: {
			par_shapes_mesh* plane = par_shapes_create_plane(5, 5);
			LoadMeshPrimitive(plane);
			par_shapes_free_mesh(plane);
		}
			break;
		}

		BindBuffersPrimitive();
	}

	return true;
}

bool ResourceMesh::UnloadInMemory()
{
	glDeleteBuffers(1, (GLuint*) &(id_index));
	glDeleteBuffers(1, (GLuint*) &(id_vertex));
	if(normal)
		glDeleteBuffers(1, (GLuint*) &(id_normal));
	if(uv)
		glDeleteBuffers(1, (GLuint*) &(id_uv));

	RELEASE_ARRAY(index);
	RELEASE_ARRAY(vertex);
	RELEASE_ARRAY(normal);
	RELEASE_ARRAY(uv);
	
	return true;
}

void ResourceMesh::LoadMeshPrimitive(par_shapes_mesh * shape)
{
	num_vertex = (uint)shape->npoints;
	vertex = new float[num_vertex * 3];
	memcpy(vertex, shape->points, sizeof(float) * num_vertex * 3);

	num_index = (uint)shape->ntriangles;
	index = new uint[num_index * 3];
	memcpy(index, shape->triangles, sizeof(PAR_SHAPES_T) * num_index * 3);

}

void ResourceMesh::BindBuffers()
{
	if (index != nullptr && vertex != nullptr) {
		//Vertex
		glGenBuffers(1, (GLuint*) &(id_vertex));
		glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertex, vertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Index
		glGenBuffers(1, (GLuint*) &(id_index));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index, index, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//Normal
		if (normal != nullptr) {
			glGenBuffers(1, (GLuint*) &(id_normal));
			glBindBuffer(GL_ARRAY_BUFFER, id_normal);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_normal, normal, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		//Texture coordinates
		if (uv != nullptr) {
			glGenBuffers(1, (GLuint*) &(id_uv));
			glBindBuffer(GL_ARRAY_BUFFER, id_uv);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * num_uv, uv, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}

void ResourceMesh::BindBuffersPrimitive()
{
	//Vertex
	glGenBuffers(1, (GLuint*) &(id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertex, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Index
	glGenBuffers(1, (GLuint*) &(id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * 3 * num_index, index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool ResourceMesh::IsPrimitive() const
{
	if (is_primitive == PRIMITIVE_NONE)
		return false;

	return true;
}
