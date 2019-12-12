// !preview r2d3 data=sim_basic_block_network(3, 5)

// Clean up data into the node and link format that d3 needs

const links = HTMLWidgets.dataframeToD3(data.edges)
  .map(function(edge){return({source: edge.from, target: edge.to})});
const nodes = HTMLWidgets.dataframeToD3(data.nodes);


// If we're missing groups or types fill them in with constant values.
if(!nodes[0].group){
  nodes.forEach(function(node){node.group = "group"});
}
if(!nodes[0].type){
  nodes.forEach(function(node){node.type = "type"});
}

// Color encodes the node's group
const Color = d3.scaleOrdinal(d3.schemeCategory10, unique(nodes.map(d => d.group)));
const color_node = node => Color(node.group);

// Shape encodes the node's type
const Shape = d3.scaleOrdinal(d3.symbols, unique(nodes.map(d => d.type)));
const draw_shape = node => d3.symbol().size(150)(Shape(node.type));

// Setup the simulation
const simulation = d3.forceSimulation(nodes)
      .force("link", d3.forceLink(links).id(d => d.id))
      .force("charge", d3.forceManyBody())
      .force("center", d3.forceCenter(width / 2, height / 2));

// Setup the node and link visual components
const link = svg.append("g")
  .attr("stroke", "#999")
  .attr("stroke-opacity", 0.6)
  .selectAll("line")
  .data(links)
  .enter().append("line")
  .attr("stroke-width", 1);

const node = svg.append("g")
  .attr("stroke", "#fff")
  .selectAll("path.node")
  .data(nodes)
  .enter().append('path')
  .classed('node', true)
  .attr('fill', color_node)
  .attr('d', draw_shape)
  .call(drag(simulation));

// Kickoff simulation
simulation.on("tick", () => {
  link
    .attr("x1", d => d.source.x)
    .attr("y1", d => d.source.y)
    .attr("x2", d => d.target.x)
    .attr("y2", d => d.target.y);

  node.attr('transform', d => `translate(${d.x}, ${d.y})`);
});


function drag(simulation){
  function dragstarted(d) {
    if (!d3.event.active) simulation.alphaTarget(0.3).restart();
    d.fx = d.x;
    d.fy = d.y;
  }

  function dragged(d) {
    d.fx = d3.event.x;
    d.fy = d3.event.y;
  }

  function dragended(d) {
    if (!d3.event.active) simulation.alphaTarget(0);
    d.fx = null;
    d.fy = null;
  }

  return d3.drag()
      .on("start", dragstarted)
      .on("drag", dragged)
      .on("end", dragended);
}


function unique (vals){
  return [...new Set(vals)];
}


