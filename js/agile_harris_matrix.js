(function ($) {
    'use strict';
    Drupal.behaviors.agileHarrisMatrix = {
        attach: function (context, settings) {
            $('#tax_link').hide();
            $('#tax_link').on("click", function (d) {
                $('#tax_link').hide();
            });
            function viewport() {
                var e = window,
                        a = 'inner';
                if (!('innerWidth' in window)) {
                    a = 'client';
                    e = document.documentElement || document.body;
                }
                return {
                    width: e[a + 'Width'],
                    height: e[a + 'Height']
                }
            }
            var callback_url = Drupal.settings.basePath + 'agile/term_callback';
            var width = viewport().width,
                    height = viewport().height;
            var tax_path = '';
            var zoom = d3.behavior.zoom()
                    .on("zoom", redraw);
            var svg = d3.select("#viz")
                    .append("svg")
                    .attr("width", width)
                    .attr("height", height)
                    .call(zoom)
                    .append("g");

// group
            var root = svg.append("g");
            var layouter = klay.d3kgraph()
                    .size([width, height])
                    .transformGroup(root)
                    .options({
                        algorithm: "de.cau.cs.kieler.klay.layered",
                        layoutHierarchy: true,
                        intCoordinates: true,
                        direction: "DOWN",
                        edgeRouting: "ORTHOGONAL",
                        nodeLayering: "NETWORK_SIMPLEX",
                        separateConnComp: false
                    });


// load data and render elements
            $('#tooltip').tooltipster({
                contentAsHTML: true,
                interactive: true
            });

            $('#tax_link').tooltipster({
                theme: 'tooltipster-light',
                contentAsHTML: true,
                interactive: true,
                animation: 'fade',
                delay: 200
            });
            d3.json(Drupal.settings.agile_harris_matrix.json_address, function (error, graph) {

                layouter.on("finish", function (d) {
                    var nodes = layouter.nodes();
                    var links = layouter.links(nodes);

                    var linkData = root.selectAll(".link")
                            .data(links, function (d) {
                                return d.id;

                            });

                    // build the arrow.
                    svg.append("svg:defs").selectAll("marker")
                            .data(["end"])                 // define link/path types
                            .enter().append("svg:marker")    // add arrows
                            .attr("id", String)
                            .attr("viewBox", "0 -5 10 10")
                            .attr("refX", 10)
                            .attr("refY", 0)
                            .attr("markerWidth", 5)        // marker settings
                            .attr("markerHeight", 5)
                            .attr("orient", "auto")
                            .style("fill", "#000")         // arrowhead color
                            .style("opacity", 0.8)
                            .append("svg:path")
                            .attr("d", "M0,-5L10,0L0,5");

                    // add arrows and colors
                    var link = linkData.enter()
                            .append("path")
                            .attr("class", "link")
                            .attr("stroke", function (d) {  // color contemporary edges dashed and red
                                return d.type == "CONTEMPORARY" ? "#FF0000" : "#000";
                            })
                            .style("stroke-dasharray", function (d) {
                                if (d.type == "CONTEMPORARY" || d.type == "LATER") {
                                    return ("3, 3");
                                }
                            })
                            .attr("d", "M0 0")
                            .attr("marker-end", function (d) {
                                if (d.type != "CONTEMPORARY") {
                                    return "url(#end)";
                                }
                            });

                    var nodeData = root.selectAll(".node")
                            .data(nodes, function (d) {
                                return d.id;
                            });
                    var node = nodeData.enter()
                            .append("g")
                            .attr("class", function (d) {
                                if (d.children)
                                    return "node compound";
                                else
                                    return "node leaf";
                            })
                            .on("mouseover", function (d) {
                                console.log(callback_url);
                                $.ajax({
                                    url: callback_url,
                                    type: "POST",
                                    data: {
                                        vocabulary: Drupal.settings.agile_harris_matrix.vocabulary,
                                        term: d.id

                                    },
                                    async: false,
                                    success: function (results, status, xhr) {
                                        tax_path = results;
                                    },
                                    error: function (data, status, xhd) {
                                        console.log("The function execute_callback has failed");
                                    }
                                });


                                $('#tax_link').tooltipster('content', '<a href = " '+ Drupal.settings.basePath + tax_path + '">More about '+ d.id + '</a>');
                                $('#tax_link').css({
                                    'position': 'relative',
                                    'left': (d3.event.pageX - 430) + "px",
                                    'top': (d3.event.pageY - 300) + "px"
                                }).html("<strong>ID:</strong>" + d.id + "<br>" +
                                        "<strong>Name</strong>: " + d.name + "<br>" +
                                        "<strong>Description</strong>: " + d.description + "<br>" +
                                        "<strong>Type</strong>: " + d.type).show();
                            })


                    var atoms = node.append("rect")
                            .attr("width", 10)
                            .attr("height", 10);

                    // add node labels
                    node.append("text")
                            .attr("x", 1)
                            .attr("y", 7)
                            .text(function (d) {
                                return d.id;
                            })
                            .attr("font-size", "6px");

                    // apply edge routes
                    link.transition().attr("d", function (d) {
                        var path = "";
                        path += "M" + d.sourcePoint.x + " " + d.sourcePoint.y + " ";
                        (d.bendPoints || []).forEach(function (bp, i) {
                            path += "L" + bp.x + " " + bp.y + " ";
                        });
                        path += "L" + d.targetPoint.x + " " + d.targetPoint.y + " ";
                        return path;
                    });

                    // apply node positions
                    node.transition()
                            .attr("transform", function (d) {
                                return "translate(" + (d.x || 0) + " " + (d.y || 0) + ")";
                            });

                    atoms.transition()
                            .attr("width", function (d) {
                                return d.width;
                            })
                            .attr("height", function (d) {
                                return d.height;
                            });

                    // remove root node
                    d3.selectAll(".node").each(function (d, i) {
                        if (d.id == "root") {
                            this.remove();
                        }
                    });

                });

                layouter.kgraph(graph);
                $('.interactive_hover').tooltipster({
                    contentAsHTML: true,
                    interactive: true
                });
            });

            function redraw() {
                svg.attr("transform", "translate(" + d3.event.translate + ")"
                        + " scale(" + d3.event.scale + ")");
            }

        }
    };
})(jQuery);

