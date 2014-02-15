/*
 * Material.h
 *
 *  Created on: 28.10.2008
 *      Author: Wictor
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <gl/gl.h>
#include <boost/shared_ptr.hpp>

///\details Класс, инкапсулирующий параметры материала
struct Material
{
	///\details Параметры для GL_AMBIENT
	GLfloat mat_dif[4];
	///\details Параметры для GL_DIFFUSE
	GLfloat mat_amb[4];
	///\details Параметры для GL_SPECULAR
	GLfloat mat_spec[4];
	///\details Параметр для GL_SHININESS
	GLfloat mat_shininess;
	///\details Параметры для GL_EMISSION
	GLfloat mat_emission[4];

	///\details Конструктор, в котором задаются параметры материала
	Material(GLfloat mat_amb1,
			GLfloat mat_amb2,
			GLfloat mat_amb3,
			GLfloat mat_amb4,
			GLfloat mat_dif1,
			GLfloat mat_dif2,
			GLfloat mat_dif3,
			GLfloat mat_dif4,
			GLfloat mat_spec1,
			GLfloat mat_spec2,
			GLfloat mat_spec3,
			GLfloat mat_spec4,
			GLfloat mat_shininess_,
			GLfloat mat_emission1=0,
			GLfloat mat_emission2=0,
			GLfloat mat_emission3=0,
			GLfloat mat_emission4=1
			)
	{
		mat_dif[0]=(mat_dif1),
		mat_dif[1]=(mat_dif2),
		mat_dif[2]=(mat_dif3),
		mat_dif[3]=(mat_dif4),
		mat_amb[0]=(mat_amb1),
		mat_amb[1]=(mat_amb2),
		mat_amb[2]=(mat_amb3),
		mat_amb[3]=(mat_amb4),
		mat_spec[0]=(mat_spec1),
		mat_spec[1]=(mat_spec2),
		mat_spec[2]=(mat_spec3),
		mat_spec[3]=(mat_spec4),
		mat_shininess=(mat_shininess_),
		mat_emission[0]=(mat_emission1),
		mat_emission[1]=(mat_emission2),
		mat_emission[2]=(mat_emission3),
		mat_emission[3]=(mat_emission4);
	}

	///\details Функция, делающая данный материал текущим
	void apply(GLenum side = GL_FRONT) const
	{
		glMaterialfv (side,GL_AMBIENT,mat_amb);
		glMaterialfv (side,GL_DIFFUSE,mat_dif);
		glMaterialfv (side,GL_SPECULAR,mat_spec);
		glMaterialfv (side,GL_EMISSION,mat_emission);
		glMaterialf (side,GL_SHININESS,mat_shininess);
	}

	void setAlpha(GLfloat alpha)
	{
		mat_amb[3] = alpha;
		mat_dif[3] = alpha;
		mat_spec[3] = alpha;
		mat_emission[3] = alpha;
	}

};

///\details Тип указателя на материал
typedef boost::shared_ptr<Material> PMaterial;

#endif /* MATERIAL_H_ */
