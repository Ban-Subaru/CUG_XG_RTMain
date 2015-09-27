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



#ifndef OSG_DELAUNAYTRIANGULATOR_H_
#define OSG_DELAUNAYTRIANGULATOR_H_

#include <list>
#include <unordered_map>
#include "Geometry.h"
#include "Array.h"
#include "Tessellator.h"
#include "ReadDEM.h"
namespace osg
{


	// Compute the circumcircle of a triangle (only x and y coordinates are used),
	// return (Cx, Cy, r^2)
	inline Vec3 compute_circumcircle(
		const Vec3 &a,
		const Vec3 &b,
		const Vec3 &c)
	{
		float D =
			(a.x() - c.x()) * (b.y() - c.y()) -
			(b.x() - c.x()) * (a.y() - c.y());

		float cx, cy, r2;

		if (D == 0.0)
		{
			// (Nearly) degenerate condition - either two of the points are equal (which we discount)
			// or the three points are colinear. In this case we just determine the average of
			// the three points as the centre for correctness, but squirt out a zero radius.
			// This method will produce a triangulation with zero area, so we have to check later
			cx = (a.x() + b.x() + c.x()) / 3.0;
			cy = (a.y() + b.y() + c.y()) / 3.0;
			r2 = 0.0;
		}
		else
		{
			cx =
				(((a.x() - c.x()) * (a.x() + c.x()) +
				(a.y() - c.y()) * (a.y() + c.y())) / 2 * (b.y() - c.y()) -
				((b.x() - c.x()) * (b.x() + c.x()) +
				(b.y() - c.y()) * (b.y() + c.y())) / 2 * (a.y() - c.y())) / D;

			cy =
				(((b.x() - c.x()) * (b.x() + c.x()) +
				(b.y() - c.y()) * (b.y() + c.y())) / 2 * (a.x() - c.x()) -
				((a.x() - c.x()) * (a.x() + c.x()) +
				(a.y() - c.y()) * (a.y() + c.y())) / 2 * (b.x() - c.x())) / D;

			//  r2 = (c.x() - cx) * (c.x() - cx) + (c.y() - cy) * (c.y() - cy);
			// the return r square is compared with r*r many times in an inner loop
			// so for efficiency use the inefficient sqrt once rather than 30* multiplies later.
			r2 = sqrt((c.x() - cx) * (c.x() - cx) + (c.y() - cy) * (c.y() - cy));
		}
		return Vec3(cx, cy, r2);
	}

	// Test whether a point (only the x and y coordinates are used) lies inside
	// a circle; the circle is passed as a vector: (Cx, Cy, r).

	inline bool point_in_circle(const Vec3 &point, const Vec3 &circle)
	{
		float r2 =
			(point.x() - circle.x()) * (point.x() - circle.x()) +
			(point.y() - circle.y()) * (point.y() - circle.y());
		return r2 <= circle.z()*circle.z();
		//    return r2 <= circle.z();
	}


	//
	//////////////////////////////////////////////////////////////////////////////////////


	// data type for vertex indices
	typedef GLuint Vertex_index;



	// CLASS: Edge
	// This class describes an edge of a triangle (it stores vertex indices to the two
	// endpoints).

	class Edge {
	public:

		// Comparison object (for sorting)
		struct Less
		{
			inline bool operator()(const Edge &e1, const Edge &e2) const
			{
				if (e1.ibs() < e2.ibs()) return true;
				if (e1.ibs() > e2.ibs()) return false;
				if (e1.ies() < e2.ies()) return true;
				return false;
			}
		};

		Edge() : ib_(0), ie_(0), ibs_(0), ies_(0), duplicate_(false) {}
		Edge(Vertex_index ib, Vertex_index ie) : ib_(ib), ie_(ie), ibs_(minimum(ib, ie)), ies_(maximum(ib, ie)), duplicate_(false) {}

		// first endpoint
		inline Vertex_index ib() const { return ib_; }

		// second endpoint
		inline Vertex_index ie() const { return ie_; }

		// first sorted endpoint
		inline Vertex_index ibs() const { return ibs_; }

		// second sorted endpoint
		inline Vertex_index ies() const { return ies_; }

		// get the "duplicate" flag
		inline bool get_duplicate() const { return duplicate_; }

		// set the "duplicate" flag
		inline void set_duplicate(bool v) { duplicate_ = v; }

	private:
		Vertex_index ib_, ie_;
		Vertex_index ibs_, ies_;
		bool duplicate_;
	};




	// comparison function for sorting sample points by the X coordinate
	


	// container types
	typedef std::set<Edge, Edge::Less> Edge_set;



	// CLASS: Triangle

	class Triangle
	{
	public:

		Triangle() :
			a_(0),
			b_(0),
			c_(0) {}

		Triangle(Vertex_index a, Vertex_index b, Vertex_index c)
		{
			a_ = a;
			b_ = b;
			c_ = c;
		}

		Triangle(Vertex_index a, Vertex_index b, Vertex_index c, Vec3Array *points)
			: a_(a),
			b_(b),
			c_(c),
			cc_(compute_circumcircle((*points)[a_], (*points)[b_], (*points)[c_]))
		{
			edge_[0] = Edge(a_, b_);
			edge_[1] = Edge(b_, c_);
			edge_[2] = Edge(c_, a_);
		}

		/*//--bool operator < (const Triangle& p) const
		{
			int a,  b;
			int ap, bp;
			
			if (a_ > b_)
			{
				if (a_ > c_)
				{
					a = a_;
				}
				else
				{
					a = c_;
				}
			}
			else
			{
				if (b_ > c_)
				{
					a = b_;
				}
				else
				{
					a = c_;
				}
			}
			//
			if (a_ < b_)
			{
				if (a_ < c_)
				{
					b = a_;
				}
				else
				{
					b = c_;
				}
			}
			else
			{
				if (b_ < c_)
				{
					b = b_;
				}
				else
				{
					b = c_;
				}
			}
			//---------------------------------
			if (p.a_ > p.b_)
			{
				if (p.a_ > p.c_)
				{
					ap = p.a_;
				}
				else
				{
					ap = p.c_;
				}
			}
			else
			{
				if (p.b_ > p.c_)
				{
					ap = p.b_;
				}
				else
				{
					ap = p.c_;
				}
			}
			//
			if (p.a_ < p.b_)
			{
				if (p.a_ < p.c_)
				{
					bp = p.a_;
				}
				else
				{
					bp = p.c_;
				}
			}
			else
			{
				if (p.b_ < p.c_)
				{
					bp = p.b_;
				}
				else
				{
					bp = p.c_;
				}
			}


			if (a < ap && b < bp)
				return a_ < p.a_;
		}*/

		Triangle& operator = (const Triangle& rhs)
		{
			if (&rhs == this) return *this;

			a_ = rhs.a_;
			b_ = rhs.b_;
			c_ = rhs.c_;
			cc_ = rhs.cc_;
			edge_[0] = rhs.edge_[0];
			edge_[1] = rhs.edge_[1];
			edge_[2] = rhs.edge_[2];

			return *this;
		}

		bool operator==(const Triangle &tri1) const
		{
			if (a_ == tri1.a_ || a_ == tri1.b_ || a_ == tri1.c_)
			{
				if (b_ == tri1.a_ || b_ == tri1.b_ || b_ == tri1.c_)
				{
					if (c_ == tri1.a_ || c_ == tri1.b_ || c_ == tri1.c_)
						return true;
				}
			}
			return false;
		}

		inline Vertex_index a() const { return a_; }
		inline Vertex_index b() const { return b_; }
		inline Vertex_index c() const { return c_; }
		inline void incrementa(const int delta) { a_ += delta; }
		inline void incrementb(const int delta) { b_ += delta; }
		inline void incrementc(const int delta) { c_ += delta; }

		inline const Edge &get_edge(int idx) const { return edge_[idx]; }
		inline const Vec3 &get_circumcircle() const { return cc_; }

		inline Vec3 compute_centroid(const Vec3Array *points) const
		{
			return ((*points)[a_] + (*points)[b_] + (*points)[c_]) / 3;
		}

		inline Vec3 compute_normal(Vec3Array *points) const
		{
			Vec3 N = ((*points)[b_] - (*points)[a_]) ^ ((*points)[c_] - (*points)[a_]);
			return N / N.length();
		}

		bool isedge(const unsigned int ip1, const unsigned int ip2) const
		{ // is one of the edges of this triangle from ip1-ip2
			bool isedge = ip1 == a() && ip2 == b();
			if (!isedge)
			{
				isedge = ip1 == b() && ip2 == c();
				if (!isedge)
				{
					isedge = ip1 == c() && ip2 == a();
				}
			}
			return isedge;
		}
		// GWM July 2005 add test for triangle intersected by p1-p2.
		// return true for unused edge

		const bool intersected(const unsigned int ip1, const unsigned int ip2, const Vec2 p1, const Vec2 p2, const int iedge, Vec3Array *points) const
		{
			// return true if edge iedge of triangle is intersected by ip1,ip2
			Vertex_index ie1, ie2;
			if (iedge == 0)
			{
				ie1 = a();
				ie2 = b();
			}
			else if (iedge == 1)
			{
				ie1 = b();
				ie2 = c();
			}
			else if (iedge == 2)
			{
				ie1 = c();
				ie2 = a();
			}
			if (ip1 == ie1 || ip2 == ie1) return false;
			if (ip1 == ie2 || ip2 == ie2) return false;

			Vec2 tp1((*points)[ie1].x(), (*points)[ie1].y());
			Vec2 tp2((*points)[ie2].x(), (*points)[ie2].y());
			return intersect(tp1, tp2, p1, p2);
		}

		bool intersectedby(const Vec2 p1, const Vec2 p2, Vec3Array *points) const {
			// true if line [p1,p2] cuts at least one edge of this triangle
			Vec2 tp1((*points)[a()].x(), (*points)[a()].y());
			Vec2 tp2((*points)[b()].x(), (*points)[b()].y());
			Vec2 tp3((*points)[c()].x(), (*points)[c()].y());
			bool ip = intersect(tp1, tp2, p1, p2);
			if (!ip)
			{
				ip = intersect(tp2, tp3, p1, p2);
				if (!ip)
				{
					ip = intersect(tp3, tp1, p1, p2);
				}
			}
			return ip;
		}
		int whichEdge(Vec3Array *points, const Vec2 p1, const Vec2 p2,
			const unsigned int e1, const unsigned int e2) const
		{
			int icut = 0;
			// find which edge of triangle is cut by line (p1-p2) and is NOT e1-e2 indices.
			// return 1 - cut is on edge b-c; 2==c-a
			Vec2 tp1((*points)[a()].x(), (*points)[a()].y()); // triangle vertices
			Vec2 tp2((*points)[b()].x(), (*points)[b()].y());
			Vec2 tp3((*points)[c()].x(), (*points)[c()].y());
			bool ip = intersect(tp2, tp3, p1, p2);
			if (ip && (a() == e1 || a() == e2)) { return 1; }
			ip = intersect(tp3, tp1, p1, p2);
			if (ip && (b() == e1 || b() == e2)) { return 2; }
			ip = intersect(tp1, tp2, p1, p2);
			if (ip && (c() == e1 || c() == e2)) { return 3; }
			return icut;
		}

		bool usesVertex(const unsigned int ip) const
		{
			return ip == a_ || ip == b_ || ip == c_;
		}

		int lineBisectTest(const Vec3 apt, const Vec3 bpt, const Vec3 cpt, const Vec2 p2) const
		{
			Vec2 vp2tp = p2 - Vec2(apt.x(), apt.y()); // vector from p1 to a.
			// test is: cross product (z component) with ab,ac is opposite signs
			// AND dot product with ab,ac has at least one positive value.
			Vec2 vecba = Vec2(bpt.x(), bpt.y()) - Vec2(apt.x(), apt.y());
			Vec2 vecca = Vec2(cpt.x(), cpt.y()) - Vec2(apt.x(), apt.y());
			float cprodzba = vp2tp.x()*vecba.y() - vp2tp.y()*vecba.x();
			float cprodzca = vp2tp.x()*vecca.y() - vp2tp.y()*vecca.x();
			//    OSG_WARN << "linebisect test " << " tri " << a_<<","<< b_<<","<< c_<<std::endl;
			if (cprodzba*cprodzca<0)
			{
				// more tests - check dot products are at least partly parallel to test line.
				Vec2 tp1(bpt.x(), bpt.y()); // triangle vertices
				Vec2 tp2(cpt.x(), cpt.y());
				Vec2 tp3(apt.x(), apt.y());
				bool ip = intersect(tp1, tp2, tp3, p2);
				if (ip) return 1;
			}
			return 0;
		}

		int lineBisects(Vec3Array *points, const unsigned int ip1, const Vec2 p2) const
		{
			// return true if line starts at vertex <ip1> and lies between the 2 edges which meet at vertex
			// <vertex> is that which uses index ip1.
			// line is <vertex> to p2
			//    return value is 0 - no crossing; 1,2,3 - which edge of the triangle is cut.
			if (a_ == ip1)
			{
				// first vertex is the vertex - test that a_ to p2 lies beteen edges a,b and a,c
				Vec3 apt = (*points)[a_];
				Vec3 bpt = (*points)[b_];
				Vec3 cpt = (*points)[c_];
				return lineBisectTest(apt, bpt, cpt, p2) ? 1 : 0;
			}
			else if (b_ == ip1)
			{
				// second vertex is the vertex - test that b_ to p2 lies beteen edges a,b and a,c
				Vec3 apt = (*points)[b_];
				Vec3 bpt = (*points)[c_];
				Vec3 cpt = (*points)[a_];
				return lineBisectTest(apt, bpt, cpt, p2) ? 2 : 0;
			}
			else if (c_ == ip1)
			{
				// 3rd vertex is the vertex - test that c_ to p2 lies beteen edges a,b and a,c
				Vec3 apt = (*points)[c_];
				Vec3 bpt = (*points)[a_];
				Vec3 cpt = (*points)[b_];
				return lineBisectTest(apt, bpt, cpt, p2) ? 3 : 0;
			}
			return 0;
		}

	private:


		bool intersect(const Vec2 p1, const Vec2 p2, const Vec2 p3, const Vec2 p4) const
		{
			// intersection point of p1,p2 and p3,p4
			// test from http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/
			// the intersection must be internal to the lines, not an end point.
			float det = ((p4.y() - p3.y())*(p2.x() - p1.x()) - (p4.x() - p3.x())*(p2.y() - p1.y()));
			if (det != 0)
			{
				// point on line is P=p1+ua.(p2-p1) and Q=p3+ub.(p4-p3) 
				float ua = ((p4.x() - p3.x())*(p1.y() - p3.y()) - (p4.y() - p3.y())*(p1.x() - p3.x())) / det;
				float ub = ((p2.x() - p1.x())*(p1.y() - p3.y()) - (p2.y() - p1.y())*(p1.x() - p3.x())) / det;
				if (ua> 0.00 && ua< 1 && ub> 0.0000  && ub< 1)
				{
					return true;
				}
			}
			return false;
		}

		Vertex_index a_;
		Vertex_index b_;
		Vertex_index c_;
		Vec3 cc_;
		Edge edge_[3];
	};

	typedef std::list<Triangle> Triangle_list;

	

	class  DelaunayConstraint : public Geometry
	{
		// controls the edges in a Delaunay triangulation.
		// constraints can be linear (with width), areal (contains an area)
		// uses: to replace part of a terrain with an alternative textured model (roads, lakes).
		// the primitive sets in this are either LINE_LOOP or LINE_STRIP
	public:
		DelaunayConstraint() { }

		void addtriangle(int i1, int i2, int i3);

		Vec3Array *getPoints(const Vec3Array *points);

	protected:
		virtual ~DelaunayConstraint() {}

		typedef std::vector< int* > trilist; // array of indices in points array defining triangles

		trilist _interiorTris; // list of triangles that fits the area.

	};


	class DelaunayTriangulator
	{
	public:

		DelaunayTriangulator();
		DelaunayTriangulator(Vec3Array *points, Vec3Array *normals = 0);

		typedef std::vector<DelaunayConstraint*> linelist;

		void addInputConstraint(DelaunayConstraint *dc) { constraint_lines.push_back(dc); }

		
		/** Start triangulation. */
		std::vector<GLuint>& triangulate(Vec3Array* &points);
		//将已生成的三角网做面型地形化处理
		//--void CreateTerrain(ReadDEM *rd, Triangle_list *triangles);

	protected:
		virtual ~DelaunayTriangulator();
		DelaunayTriangulator &operator=(const DelaunayTriangulator &) { return *this; }
		int getindex(const Vec3 &pt, const Vec3Array *points);

	private:
		Vec3Array *points_;
		Vec3Array *normals_;
		std::vector<GLuint> pt_indices;
		// GWM these lines provide required edges in the triangulated shape.
		linelist constraint_lines;

		void _uniqueifyPoints();
	};

	// INLINE METHODS



}

#endif

