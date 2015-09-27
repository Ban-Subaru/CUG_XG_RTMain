/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
*
* This library is open source and may be redistributed and/or modified under
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* OpenSceneGraph Public License for more details.
*/



/*
*Editing by Subaru 
*/




//-#include "DXUT.h"
#include "DelaunayTriangulator.h"
// NB this algorithm makes heavy use of the osgUtil::Tessellator for constrained triangulation.
// truly it is built on the shoulders of giants.

#include <algorithm>
#include <set>
#include <map> //GWM July 2005 map is used in constraints.
#include <stdlib.h>
#include <iterator>

namespace osg
{

	//////////////////////////////////////////////////////////////////////////////////////
	// MISC MATH FUNCTIONS

	bool Sample_point_compare(const Vec3 &p1, const Vec3 &p2)
	{
		// replace pure sort by X coordinate with X then Y.
		// errors can occur if the delaunay triangulation specifies 2 points at same XY and different Z
		if (p1.x() != p2.x()) return p1.x() < p2.x();
		if (p1.y() != p2.y()) return p1.y() < p2.y(); // GWM 30.06.05 - further rule if x coords are same.
		//--OSG_INFO << "Two points are coincident at " << p1.x() << "," << p1.y() << std::endl;
		return p1.z() < p2.z(); // never get here unless 2 points coincide
	}


	DelaunayTriangulator::DelaunayTriangulator()
	{
		}

	DelaunayTriangulator::DelaunayTriangulator(Vec3Array *points, Vec3Array *normals) :
		points_(points),
		normals_(normals)
	{
		}

	DelaunayTriangulator::~DelaunayTriangulator()
	{
	}

	const Triangle * getTriangleWithEdge(const unsigned int ip1, const unsigned int ip2, const Triangle_list *triangles)
	{ // find triangle in list with edge from ip1 to ip2...
		Triangle_list::const_iterator trconnitr; // connecting triangle
		int idx = 0;
		for (trconnitr = triangles->begin(); trconnitr != triangles->end();)
		{
			if (trconnitr->isedge(ip1, ip2))
			{
				// this is the triangle.
				return &(*trconnitr);
			}
			++trconnitr;
			idx++;
		}
		return NULL; //-1;
	}

	int DelaunayTriangulator::getindex(const Vec3 &pt, const Vec3Array *points)
	{
		// return index of pt in points (or -1)
		for (unsigned int i = 0; i<points->size(); i++)
		{
			if (pt.x() == (*points)[i].x() && pt.y() == (*points)[i].y())
			{
				return i;
			}
		}
		return -1;
	}

	template <typename TVector>
	void removeIndices(TVector& elements, unsigned int index)
	{
		typename TVector::iterator itr = elements.begin();
		while (itr != elements.end())
		{
			if ((*itr) == index)
			{ // remove entirely
				itr = elements.erase(itr);
			}
			else
			{
				if ((*itr)>index) --(*itr); // move indices down 1
				++itr; // next index
			}
		}
	}


	Triangle_list fillHole(osg::Vec3Array *points, std::vector<unsigned int> vindexlist)
	{
		// eg clockwise vertex neighbours around the hole made by the constraint
		Triangle_list triangles; // returned list
		Geometry *gtess = new osg::Geometry; // add all the contours to this for analysis
		Vec3Array * constraintverts = new osg::Vec3Array;
		Tessellator *tscx = new Tessellator; // this assembles all the constraints

		for (std::vector<unsigned int>::iterator itint = vindexlist.begin(); itint != vindexlist.end(); itint++)
		{
			//    OSG_WARN<< "add point " << (*itint) << " at " << (*points)[*itint].x() << ","<< (*points)[*itint].y() <<std::endl;
			constraintverts->push_back((*points)[*itint]);
		}

		unsigned int npts = vindexlist.size();

		gtess->setVertexArray(constraintverts);
		gtess->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, npts));
		tscx->setTessellationNormal(osg::Vec3(0.0, 0.0, 1.0));
		tscx->setTessellationType(Tessellator::TESS_TYPE_GEOMETRY);
		tscx->setBoundaryOnly(false);
		tscx->setWindingType(Tessellator::TESS_WINDING_ODD); // the commonest tessellation is default, ODD. GE2 allows intersections of constraints to be found.
		tscx->retessellatePolygons(*(gtess)); // this should insert extra vertices where constraints overlap

		// extract triangles from gtess

		unsigned int ipr;
		for (ipr = 0; ipr<gtess->getNumPrimitiveSets(); ipr++)
		{
			unsigned int ic;
			osg::PrimitiveSet* prset = gtess->getPrimitiveSet(ipr);
			//                    OSG_WARN<< "gtess set " << ipr << " nprims " << prset->getNumPrimitives() <<
			//                        " type " << prset->getMode() << std::endl;
			unsigned int pidx, pidx1, pidx2;
			switch (prset->getMode()) {
			case osg::PrimitiveSet::TRIANGLES:
				for (ic = 0; ic<prset->getNumIndices() - 2; ic += 3)
				{
					if (prset->index(ic) >= npts)
					{
						// this is an added point.
						points->push_back((*constraintverts)[prset->index(ic)]);
						pidx = points->size() - 1;
					}
					else
					{
						pidx = vindexlist[prset->index(ic)];
					}

					if (prset->index(ic + 1) >= npts)
					{
						// this is an added point.
						points->push_back((*constraintverts)[prset->index(ic + 1)]);
						pidx1 = points->size() - 1;
					}
					else
					{
						pidx1 = vindexlist[prset->index(ic + 1)];
					}

					if (prset->index(ic + 2) >= npts)
					{
						// this is an added point.
						points->push_back((*constraintverts)[prset->index(ic + 2)]);
						pidx2 = points->size() - 1;
					}
					else
					{
						pidx2 = vindexlist[prset->index(ic + 2)];
					}
					triangles.push_back(Triangle(pidx, pidx1, pidx2, points));
					//                    OSG_WARN<< "vert " << prset->index(ic) << " in array"<<std::endl;
				}
				break;
			case osg::PrimitiveSet::TRIANGLE_STRIP: // 123, 234, 345...

				for (ic = 0; ic<prset->getNumIndices() - 2; ic++)
				{
					if (prset->index(ic) >= npts)
					{
						// this is an added point.
						points->push_back((*constraintverts)[prset->index(ic)]);
						pidx = points->size() - 1;
					}
					else {
						pidx = vindexlist[prset->index(ic)];
					}
					if (prset->index(ic + 1) >= npts)
					{
						// this is an added point.
						points->push_back((*constraintverts)[prset->index(ic + 1)]);
						pidx1 = points->size() - 1;
					}
					else
					{
						pidx1 = vindexlist[prset->index(ic + 1)];
					}

					if (prset->index(ic + 2) >= npts)
					{
						// this is an added point.
						points->push_back((*constraintverts)[prset->index(ic + 2)]);
						pidx2 = points->size() - 1;
					}
					else
					{
						pidx2 = vindexlist[prset->index(ic + 2)];
					}

					if (ic % 2 == 0)
					{
						triangles.push_back(Triangle(pidx, pidx1, pidx2, points));
					}
					else
					{
						triangles.push_back(Triangle(pidx1, pidx, pidx2, points));
					}
					//                    OSG_WARN<< "vert " << prset->index(ic) << " in array"<<std::endl;
				}
				break;

			case osg::PrimitiveSet::TRIANGLE_FAN:
			{
													if (prset->index(0) >= npts)
													{
														// this is an added point.
														points->push_back((*constraintverts)[prset->index(0)]);
														pidx = points->size() - 1;
													}
													else
													{
														pidx = vindexlist[prset->index(0)];
													}
													//        OSG_WARN<< "tfan has " << prset->getNumIndices() << " indices"<<std::endl;
													for (ic = 1; ic<prset->getNumIndices() - 1; ic++)
													{
														if (prset->index(ic) >= npts)
														{
															// this is an added point.
															points->push_back((*constraintverts)[prset->index(ic)]);
															pidx1 = points->size() - 1;
														}
														else
														{
															pidx1 = vindexlist[prset->index(ic)];
														}

														if (prset->index(ic + 1) >= npts)
														{ // this is an added point.
															points->push_back((*constraintverts)[prset->index(ic + 1)]);
															pidx2 = points->size() - 1;
														}
														else
														{
															pidx2 = vindexlist[prset->index(ic + 1)];
														}
														triangles.push_back(Triangle(pidx, pidx1, pidx2, points));
													}
			}
				break;
			default:
				break;
			}
		}
		return triangles;
	}



	void DelaunayTriangulator::_uniqueifyPoints()
	{
		std::sort(points_->begin(), points_->end());

		Vec3Array *temppts = new Vec3Array;
		// This won't work... must write our own unique() that compares only the first
		// two terms of a Vec3 for equivalency
		//std::insert_iterator< Vec3Array > ti( *(temppts.get()), temppts->begin() );
		//std::unique_copy( points_->begin(), points_->end(), ti );

		Vec3Array::iterator p = points_->begin();
		Vec3 v = *p;
		// Always push back the first point
		temppts->push_back((v = *p));
		for (; p != points_->end(); p++)
		{
			if (v[0] == (*p)[0] && v[1] == (*p)[1])
				continue;

			temppts->push_back((v = *p));
		}

		points_->clear();
		std::insert_iterator< Vec3Array > ci(*(points_), points_->begin());
		std::copy(temppts->begin(), temppts->end(), ci);
	}

	DelaunayConstraint *getconvexhull(Vec3Array *points)
	{ // fits the 'rubberband' around the 2D points for uses as a delaunay constraint.
		DelaunayConstraint* dcconvexhull = new DelaunayConstraint; // make
		// convex hull around all the points
		// start from first point (at minx); proceed to last x and back
		Vec3Array *verts = new Vec3Array; // the hull points
		verts->push_back(*(points->begin())); // min x/y point is guaranteed to be on the hull
		verts->push_back(*(points->begin() + 1)); // second low x/y point is first length to be tested
		for (Vec3Array::iterator vit = (points->begin() + 2); vit != points->end(); vit++) {
			// check if point lies outside the current last line segment
			bool ok = 1;
			while (ok && verts->size()>1) {
				Vec3 lastseg = *(verts->end() - 2) - *(verts->end() - 1);
				Vec3 thisseg = (*vit) - *(verts->end() - 1);
				float cosang = (lastseg^thisseg).z();
				if (cosang <0.0) { // pop off last point - *vit is further out hull
					verts->pop_back();
				}
				else { ok = 0; }
			}
			verts->push_back(*vit); // next low x/y point is next length to be tested
			// check if the previous external angle is >180 - then remove previous point
		}
		for (Vec3Array::reverse_iterator rvit = points->rbegin() + 1; rvit != points->rend(); rvit++) {
			// check if point lies outside the current last line segment
			bool ok = 1;
			while (ok && verts->size()>1) {
				Vec3 lastseg = *(verts->end() - 2) - *(verts->end() - 1);
				Vec3 thisseg = (*rvit) - *(verts->end() - 1);
				float cosang = (lastseg^thisseg).z();
				if (cosang <0.0) { // pop off last point - *rvit is further out hull
					verts->pop_back();
				}
				else { ok = 0; }
			}
			if ((*rvit) != *(verts->begin())) verts->push_back(*rvit); // next low x/y point is next length to be tested
			// check if the previous external angle is >180 - then remove previous point
		}
		dcconvexhull->setVertexArray(verts);
		dcconvexhull->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, verts->size()));
		return dcconvexhull;//--
	}


	/*//--void DelaunayTriangulator::CreateTerrain(ReadDEM *rd,Triangle_list *triangles)
	{
		if (triangles->empty()) return;
		//获取边界和步长
		int vacuate = rd->Getvacuate();
		auto *size_x_y = rd->GetSize_x_y();

		if (size_x_y[0] == size_x_y[1])
		{
			auto pointsline = size_x_y[0] / vacuate;
			Triangle_list _deletetris;
			Triangle_list _addtris;

			typedef map<Triangle,int> u_map;
			u_map mtris;
			//初始化哈希map
			for (Triangle_list::iterator i = triangles->begin(); i != triangles->end(); i++)
			{
				mtris.insert(u_map::value_type(*i, triangles->size()));
			}
			
			int tris_count = 0;

			_addtris.push_back((Triangle(1,2,1 + pointsline)));
			tris_count++;

			//删除由索引为1的点和第二列的所有连线，并添加三角形
			for (int i = 2; i <( triangles->size() - pointsline ); i += pointsline)
			{
				_deletetris.push_back(Triangle(i,1,i + pointsline));
				tris_count--;

				_addtris.push_back((Triangle(i + pointsline - 1, i, i + pointsline - 1 + 1)));
				tris_count++;
			}

			//删除底部多余三角形
			for (unsigned int i = 1; i < pointsline; i++)
			{
				_deletetris.push_back(Triangle(0, i, i + 1));
				tris_count--;
			}


			//横向三角形的增删
			_deletetris.push_back(Triangle(0, 0 + pointsline + 1 - 1, 0 + pointsline + 1 - 1 + 1));
			tris_count--;
			
			_addtris.push_back(Triangle(0,1,pointsline + 1));
			tris_count++;

			for (int i = pointsline + 1; i < (triangles->size() - pointsline); i += pointsline)
			{
				_deletetris.push_back(Triangle(i, i + pointsline - 1, i + pointsline - 1 + 1));
				tris_count--;

				_deletetris.push_back(Triangle(i, i - 1, i - 1 + pointsline));
				tris_count--;

				_addtris.push_back((Triangle(i, i + 1, i + pointsline)));
				tris_count++;
			}

			Triangle_list::iterator _detr;
			for (_detr = _deletetris.begin(); _detr != _deletetris.end();_detr++)
			{
				if (mtris.find(*_detr) != mtris.end())
				{
					mtris.erase(*_detr);
				}
			}


		}
	}*/

	std::vector<GLuint>& DelaunayTriangulator::triangulate(Vec3Array* &points)
	{
		// check validity of input array
		if (points_==NULL)
		{
			exit(1);
		}

		points = points_;

		if (points->size() < 1)
		{
			exit(1);
		}

		// Eliminate duplicate lat/lon points from input coordinates.
		_uniqueifyPoints();


		// initialize storage structures
		Triangle_list triangles;
		Triangle_list discarded_tris;
		// pre-sort sample points
		//--OSG_INFO << "DelaunayTriangulator: pre-sorting sample points\n";
		std::sort(points->begin(), points->end(), Sample_point_compare);
		// 24.12.06 add convex hull of points to force sensible outline.
		DelaunayConstraint *dcconvexhull = getconvexhull(points);
		addInputConstraint(dcconvexhull);

		// set the last valid index for the point list
		GLuint last_valid_index = points->size() - 1;

		// find the minimum and maximum x values in the point list
		float minx = (*points)[0].x();
		float maxx = (*points)[last_valid_index].x();

		// find the minimum and maximum y values in the point list    
		float miny = (*points)[0].y();
		float maxy = miny;

		Vec3Array::const_iterator mmi;
		for (mmi = points->begin(); mmi != points->end(); ++mmi)
		{
			if (mmi->y() < miny) miny = mmi->y();
			if (mmi->y() > maxy) maxy = mmi->y();
		}

		// add supertriangle vertices to the point list
		// gwm added 1.05* to ensure that supervertices are outside the domain of points.
		// original value could make 2 coincident points for regular arrays of x,y,h data.
		// this mod allows regular spaced arrays to be used.
		// 16 Dec 2006 this increase in size encourages the supervertex triangles to be long and thin
		// thus ensuring that the convex hull of the terrain points are edges in the delaunay triangulation
		// the values do however result in a small loss of numerical resolution.
		points_->push_back(Vec3(minx - .10*(maxx - minx), miny - .10*(maxy - miny), 0));
		points_->push_back(Vec3(maxx + .10*(maxx - minx), miny - .10*(maxy - miny), 0));
		points_->push_back(Vec3(maxx + .10*(maxx - minx), maxy + .10*(maxy - miny), 0));
		points_->push_back(Vec3(minx - .10*(maxx - minx), maxy + .10*(maxy - miny), 0));

		// add supertriangles to triangle list                           
		triangles.push_back(Triangle(last_valid_index + 1, last_valid_index + 2, last_valid_index + 3, points));
		triangles.push_back(Triangle(last_valid_index + 4, last_valid_index + 1, last_valid_index + 3, points));


		// begin triangulation    
		GLuint pidx = 0;
		Vec3Array::const_iterator i;

		for (i = points->begin(); i != points->end(); ++i, ++pidx)
		{

			// don't process supertriangle vertices
			if (pidx > last_valid_index) break;

			Edge_set edges;

			// iterate through triangles
			Triangle_list::iterator j, next_j;
			for (j = triangles.begin(); j != triangles.end(); j = next_j)
			{

				next_j = j;
				++next_j;

				// get the circumcircle (x,y centre & radius)
				Vec3 cc = j->get_circumcircle();

				// OPTIMIZATION: since points are pre-sorted by the X component,
				// check whether we can discard this triangle for future operations
				float xdist = i->x() - cc.x();
				// this is where the circumcircles radius rather than R^2 is faster.
				// original code used r^2 and needed to test xdist*xdist>cc.z && i->x()>cc.x().
				if ((xdist) > cc.z())
				{
					discarded_tris.push_back(*j); // these are not needed for further tests as no more
					// points will ever lie inside this triangle.
					triangles.erase(j);
				}
				else
				{

					// if the point lies in the triangle's circumcircle then add
					// its edges to the edge list and remove the triangle
					if (point_in_circle(*i, cc))
					{
						for (int ei = 0; ei<3; ++ei)
						{
							std::pair<Edge_set::iterator, bool> result = edges.insert(j->get_edge(ei));
							if (!result.second)
							{
								// cast away constness of a set element, which is
								// safe in this case since the set_duplicate is
								// not used as part of the Less operator.
								Edge& edge = const_cast<Edge&>(*(result.first));
								// not clear why this change is needed? But prevents removal of twice referenced edges??
								//      edge.set_duplicate(true);
								edge.set_duplicate(!edge.get_duplicate());
							}
						}
						triangles.erase(j);
					}
				}
			}

			// remove duplicate edges and add new triangles
			Edge_set::iterator ci;
			for (ci = edges.begin(); ci != edges.end(); ++ci)
			{
				if (!ci->get_duplicate())
				{
					triangles.push_back(Triangle(pidx, ci->ib(), ci->ie(), points));
				}
			}
		}
		// dec 2006 we used to remove supertriangle vertices here, but then we cant strictly use the supertriangle
		// vertices to find intersections of constraints with terrain, so moved to later.

		//--OSG_INFO << "DelaunayTriangulator: finalizing and cleaning up structures\n";

		// rejoin the two triangle lists
		triangles.insert(triangles.begin(), discarded_tris.begin(), discarded_tris.end());

		// GWM July 2005 eliminate any triangle with an edge crossing a constraint line
		// http://www.geom.uiuc.edu/~samuelp/del_project.html
		// we could also implement the sourcecode in http://gts.sourceforge.net/reference/gts-delaunay-and-constrained-delaunay-triangulations.html
		// this uses the set of lines which are boundaries of the constraints, including points
		// added to the contours by tessellation.

		for (linelist::iterator dcitr = constraint_lines.begin(); dcitr != constraint_lines.end(); dcitr++)
		{
			//DelaunayConstraint *dc=(*dcitr).get();
			const osg::Vec3Array* vercon = dynamic_cast<const osg::Vec3Array*>((*dcitr)->getVertexArray());
			if (vercon)
			{
				for (unsigned int ipr = 0; ipr<(*dcitr)->getNumPrimitiveSets(); ipr++)
				{
					const osg::PrimitiveSet* prset = (*dcitr)->getPrimitiveSet(ipr);
					if (prset->getMode() == osg::PrimitiveSet::LINE_LOOP ||
						prset->getMode() == osg::PrimitiveSet::LINE_STRIP)
					{
						// loops or strips
						// start with the last point on the loop
						unsigned int ip1 = getindex((*vercon)[prset->index(prset->getNumIndices() - 1)], points_);
						for (unsigned int i = 0; i<prset->getNumIndices(); i++)
						{
							unsigned int ip2 = getindex((*vercon)[prset->index(i)], points_);
							if (i>0 || prset->getMode() == osg::PrimitiveSet::LINE_LOOP)
							{
								// dont check edge from end to start
								// for strips
								// 2 points on the constraint
								bool edgused = false;// first check for exact edge indices are used.
								Triangle_list::iterator titr;
								int it = 0;
								for (titr = triangles.begin(); titr != triangles.end() && !edgused; ++titr)
								{
									//check that the edge ip1-ip2 is not already part of the triangulation.
									if (titr->isedge(ip1, ip2)) edgused = true;
									if (titr->isedge(ip2, ip1)) edgused = true;
									//        if (edgused) OSG_WARN << "Edge used in triangle " << it << " " << 
									//            titr->a()<<","<< titr->b()<<","<< titr->c()<<  std::endl;
									it++;
								}
								if (!edgused)
								{
									// then check for intermediate triangles, erase them and replace with constrained triangles.
									// find triangle with point ip1 where the 2 edges from ip1 contain the line p1-p2.
									osg::Vec2 p1((*points_)[ip1].x(), (*points_)[ip1].y()); // a constraint line joins p1-p2
									osg::Vec2 p2((*points_)[ip2].x(), (*points_)[ip2].y());
									int ntr = 0;
									std::vector<const Triangle *> trisToDelete; // array of triangles to delete from terrain.
									// form 2 lists of vertices for the edges of the hole created.
									// The hole joins vertex ip1 to ip2, and one list of edges lies to the left
									// of the line ip1-ip2m the other to the right.
									// a list of vertices forming 2 halves of the removed triangles.
									// which in turn are filled in with the tessellator.
									for (titr = triangles.begin(); titr != triangles.end();)
									{
										int icut = titr->lineBisects(points_, ip1, p2);
										//    OSG_WARN << "Testing triangle " << ntr << " "<< ip1 << " ti " <<
										//        titr->a()<< ","<<titr->b() <<"," <<titr->c() << std::endl;
										if (icut>0)
										{
											// triangle titr starts the constraint edge
											std::vector<unsigned int> edgeRight, edgeLeft;
											edgeRight.push_back(ip1);
											edgeLeft.push_back(ip1);
											//        OSG_WARN << "hole first " << edgeLeft.back()<<  " rt " << edgeRight.back()<< std::endl;
											trisToDelete.push_back(&(*titr));
											// now find the unique triangle that shares the defined edge
											unsigned int e1, e2; // indices of ends of test triangle titr
											if (icut == 1)
											{
												// icut=1 implies vertex a is not involved
												e1 = titr->b(); e2 = titr->c();
											}
											else if (icut == 2)
											{
												e1 = titr->c(); e2 = titr->a();
											}
											else if (icut == 3)
											{
												e1 = titr->a(); e2 = titr->b();
											}
											edgeRight.push_back(e2);
											edgeLeft.push_back(e1);
											//        OSG_WARN << icut << "hole edges " << edgeLeft.back()<<  " rt " << edgeRight.back()<< std::endl;
											const Triangle *tradj = getTriangleWithEdge(e2, e1, &triangles);
											if (tradj)
											{
												while (tradj && !tradj->usesVertex(ip2) && trisToDelete.size()<999)
												{
													trisToDelete.push_back(tradj);
													icut = tradj->whichEdge(points_, p1, p2, e1, e2);
													//    OSG_WARN  << ntr << " cur triedge " << icut << " " << ip1 <<
													//        " to " << ip2 << " tadj " << tradj->a()<< ","<<tradj->b() <<"," 
													//        <<tradj->c() <<std::endl;
													if (icut == 1) { e1 = tradj->b(); e2 = tradj->c(); } // icut=1 implies vertex a is not involved
													else if (icut == 2) { e1 = tradj->c(); e2 = tradj->a(); }
													else if (icut == 3) { e1 = tradj->a(); e2 = tradj->b(); }
													if (edgeLeft.back() != e1 && edgeRight.back() == e2 && e1 != ip2) {
														edgeLeft.push_back(e1);
													}
													else if (edgeRight.back() != e2 && edgeLeft.back() == e1 && e2 != ip2) {
														edgeRight.push_back(e2);
													}
													else {
									
													}
													const Triangle *previousTradj = tradj;
													tradj = getTriangleWithEdge(e2, e1, &triangles);
													if (tradj == previousTradj) {
														tradj = 0;
													}
												}
												if (trisToDelete.size() >= 900) {
									                            //
												}
											}

											// both lines end at ip2 point.
											edgeLeft.push_back(ip2);
											edgeRight.push_back(ip2);
											if (tradj) trisToDelete.push_back(tradj);
											//        OSG_WARN << icut << "hole last " << edgeLeft.back()<<  " rt " << edgeRight.back()<< std::endl;
											Triangle_list constrainedtris = fillHole(points_, edgeLeft);
											triangles.insert(triangles.begin(), constrainedtris.begin(), constrainedtris.end());
											constrainedtris = fillHole(points_, edgeRight);
											triangles.insert(triangles.begin(), constrainedtris.begin(), constrainedtris.end());

										}
										++titr;
										ntr++;
									}
									// remove the triangles list
									Triangle_list::iterator tri; // counts through triangles
									for (tri = triangles.begin(); tri != triangles.end();)
									{
										bool deleted = false;
										for (std::vector<const Triangle *>::iterator deleteTri = trisToDelete.begin();
											deleteTri != trisToDelete.end();)
										{
											if (&(*tri) == *deleteTri)
											{
												deleted = true;
												tri = triangles.erase(tri);
												deleteTri = trisToDelete.erase(deleteTri); // 24.12.06 remove from delete list.
											}
											else { deleteTri++; }
										}
										if (!deleted) ++tri;
									}
								}
							} // strip test

							ip1 = ip2; // next edge of line
						}
					}
				}
			}
		}

		// GWM Sept 2005 end
		// dec 2006 remove supertriangle vertices - IF we have added some internal vertices (see fillholes)
		// then these may not be the last vertices in the list.
		//    } else { // remove 3 super-triangle vertices more completely, moving any reference indices down.
		Triangle_list::iterator tri;
		GLuint supertriend = last_valid_index + 4;
		for (tri = triangles.begin(); tri != triangles.end();)
		{ // look for triangles using the supertriangle indices or >super indices and move down by 3.
			if ((tri->a() > last_valid_index && tri->a() <= supertriend) ||
				(tri->b() > last_valid_index && tri->b() <= supertriend) ||
				(tri->c() > last_valid_index && tri->c() <= supertriend)) {
				tri = triangles.erase(tri);
			}
			else { // move down by 3 tests
				if (tri->a() > last_valid_index) { // move index down 4
					tri->incrementa(-4);
				}
				if (tri->b() > last_valid_index) { // move down 4
					tri->incrementb(-4);
				}
				if (tri->c() > last_valid_index) { // move down 4 -- correction 21.12.06- was b() should test index c()
					tri->incrementc(-4);
				}
				++tri; // only increment tri here as the other path increments tri when tri=triangles.erase.
			}
		}
		// remove 3 supertriangle vertices from points. They may not be the last vertices in points if
		// extra points have been inserted by the constraint re-triangulation.
		points->erase(points->begin() + last_valid_index + 1, points->begin() + last_valid_index + 5);
		//last_valid_index = points->size()-1; // the last valid vertex is last point.
		// end of dec 2006 changes.

		//--CreateTerrain(rd,&triangles);

		// initialize index storage vector
		pt_indices.reserve(triangles.size() * 3);

		// build osg primitive
		Triangle_list::const_iterator ti;
		for (ti = triangles.begin(); ti != triangles.end(); ++ti)
		{

			// don't add this triangle to the primitive if it shares any vertex with
			// the supertriangle triangles have already been removed.
			// Don't need this test: ti->a() <= last_valid_index && ti->b() <= last_valid_index && ti->c() <= last_valid_index &&
			// Don't add degenerate (zero radius) triangles
			if (ti->get_circumcircle().z()>0.0)
			{

				if (normals_!= NULL)
				{
					(normals_)->push_back(ti->compute_normal(points));
				}
#ifdef _OPENGL_TO_D3D//两坐标系不同，要根据情况选择
				pt_indices.push_back(ti->c());
				pt_indices.push_back(ti->b());
				pt_indices.push_back(ti->a());

#else
				pt_indices.push_back(ti->a());
				pt_indices.push_back(ti->b());
				pt_indices.push_back(ti->c());
				
				
#endif
			}
		}

		return pt_indices;
	}


	void DelaunayConstraint::addtriangle(int i1, int i2, int i3)
	{
		// a triangle joins vertices i1,i2,i3 in the points of the delaunay triangles.
		// points is the array of poitns in the triangulator;
		// add triangle to the constraint
		int *ip = new int[3];
		ip[0] = i1;
		ip[1] = i2;
		ip[2] = i3;
		_interiorTris.push_back(ip);
	}
	Vec3Array* DelaunayConstraint::getPoints(const Vec3Array *points)
	{
		//points_ is the array of points that can be used to render the triangles in this DC.
		Vec3Array* points_ = new Vec3Array;
		trilist::iterator ti;
		for (ti = _interiorTris.begin(); ti != _interiorTris.end(); ++ti) {
			int idx = 0;
			int ip[3] = { -1, -1, -1 };
			// find if points[i1/i2/i3] already in the vertices points_
			for (Vec3Array::iterator ivert = points_->begin(); ivert != points_->end(); ivert++)
			{
				if (ip[0]<0 && *ivert == (*points)[(*ti)[0]])
				{
					(*ti)[0] = ip[0] = idx;
				}
				if (ip[1]<0 && *ivert == (*points)[(*ti)[1]])
				{
					(*ti)[1] = ip[1] = idx;
				}
				if (ip[2]<0 && *ivert == (*points)[(*ti)[2]])
				{
					(*ti)[2] = ip[2] = idx;
				}
				idx++;
			}
			if (ip[0]<0)
			{
				points_->push_back((*points)[(*ti)[0]]);
				(*ti)[0] = ip[0] = points_->size() - 1;
			}
			if (ip[1]<0)
			{
				points_->push_back((*points)[(*ti)[1]]);
				(*ti)[1] = ip[1] = points_->size() - 1;
			}
			if (ip[2]<0)
			{
				points_->push_back((*points)[(*ti)[2]]);
				(*ti)[2] = ip[2] = points_->size() - 1;
			}
		}
		return points_;
	}

} // namespace osg
