// Copyright (C) 2006  Tiago de Paula Peixoto <tiago@forked.de>
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

//  Authors: Tiago de Paula Peixoto

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;
using namespace boost;

int main(int argc, char** argv)
{
    typedef adjacency_list< vecS, vecS, directedS,
        property< vertex_color_t, int, property< vertex_name_t, string > >,
        property< edge_weight_t, double >, property< graph_name_t, string > >
        graph_t;
    graph_t g;
    dynamic_properties dp;
    dp.property("foo", get(vertex_color_t(), g));
    dp.property("weight", get(edge_weight_t(), g));
    dp.property("name", get(vertex_name_t(), g));
    boost::ref_property_map< graph_t*, std::string > gname(
        get_property(g, graph_name));
    dp.property("description", gname);

    ifstream ifile(argv[1]);
    read_graphml(ifile, g, dp);
    ifile.close();

    BOOST_TEST(num_vertices(g) == 9);
    BOOST_TEST(num_edges(g) == 9);
    BOOST_TEST(get(vertex_color_t(), g, vertex(2, g)) == 100);
    BOOST_TEST(get(vertex_color_t(), g, vertex(3, g)) == 42);
    BOOST_TEST(std::abs(get(edge_weight_t(), g,
                             edge(vertex(0, g), vertex(1, g), g).first)
                    - 0.0)
        < 0.00001);
    BOOST_TEST(std::abs(get(edge_weight_t(), g,
                             edge(vertex(1, g), vertex(2, g), g).first)
                    - 0.8)
        < 0.00001);
    BOOST_TEST(get("description", dp, &g) == "Root graph.");

    ofstream ofile("graphml_test_out.xml");
    write_graphml(ofile, g, dp);
    ofile.close();

    graph_t g2;
    dynamic_properties dp2;
    dp2.property("foo", get(vertex_color_t(), g2));
    dp2.property("weight", get(edge_weight_t(), g2));
    dp2.property("name", get(vertex_name_t(), g2));
    boost::ref_property_map< graph_t*, std::string > gname2(
        get_property(g2, graph_name));
    dp2.property("description", gname2);
    ifile.open("graphml_test_out.xml");
    read_graphml(ifile, g2, dp2);
    ifile.close();

    BOOST_TEST(num_vertices(g) == num_vertices(g2));
    BOOST_TEST(num_edges(g) == num_edges(g2));
    BOOST_TEST(get("description", dp, &g) == get("description", dp2, &g2));

    graph_traits< graph_t >::vertex_iterator v, v_end;
    for (boost::tie(v, v_end) = vertices(g); v != v_end; ++v)
        BOOST_TEST(
            get(vertex_color_t(), g, *v) == get(vertex_color_t(), g2, *v));

    graph_traits< graph_t >::edge_iterator e, e_end;
    for (boost::tie(e, e_end) = edges(g); e != e_end; ++e)
        BOOST_TEST(
            std::abs(get(edge_weight_t(), g, *e) - get(edge_weight_t(), g2, *e))
            < 0.00001);

    return boost::report_errors();
}
