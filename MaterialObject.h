/*
 * MaterialObject.h
 *
 *  Created on: 29.10.2008
 *      Author: Wictor
 */

#ifndef MATERIALOBJECT_H_
#define MATERIALOBJECT_H_

#include "MovableObject.h"
#include "Material.h"

///\details Абстрактный класс для объекта, имеющего метериал
///\details Он наследуется от MovableObject со спецификатором virtual,
///\details чтобы можно было определять классы, наследующие одновременно этот класс
///\details и длугие классы, наследующие MovableObject
class MaterialObject: public virtual MovableObject
{
protected:
	///\detailsУказатель на материал
	PMaterial material;
	///\detailsФункция, применяющая материал.
	///\detailsЕе нужно вызывать в объекте класса, унаследованном от данного, внутри функции paint()
	virtual void applyMaterial()
	{
		if(material!=PMaterial()) material->apply();
	}
public:
	///\details Конструктор, в котором задается материал
	MaterialObject(const PMaterial &material_):
		material(material_)//, enabled(true)
	{}

};

#endif /* MATERIALOBJECT_H_ */
