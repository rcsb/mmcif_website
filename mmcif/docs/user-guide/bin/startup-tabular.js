let WINDOWHEIGHT = 0;
let WINDOWWIDTH = 0;
let SIDEBAR = false;
let PREVTOGGLE = null;
let FETCH = false;
let TOPMARGIN = 20;

window.addEventListener("load", function(){
        if(location.search) {
            let search = location.search.substring(1);
            if (search === "default") {
                SIDEBAR = false;
            } else if (search === "sidebar") {
                SIDEBAR = true;
            } else {
                SIDEBAR = false;
            }
        }
        // set global variables
        WINDOWHEIGHT = window.innerHeight;
        WINDOWWIDTH = window.innerWidth;
        // populate user guide container
        if(FETCH) {
            bs_content_fetch(0);
        } else {
            onload_continued();
        }
    }
);

// load each page onto screen
function bs_content_fetch(index) {
    let pages = [
        "resources/introduction.html",
        "resources/header_and_keywords.html",
        "resources/title_and_authorship.html",
        "resources/citation.html",
        "resources/experimental_method.html",
        "resources/macromolecules.html",
        "resources/oligosaccharides.html",
        "resources/ligands.html",
        "resources/mx.html",
        "resources/3dem.html",
        "resources/nmr.html",
        "resources/assembly.html",
        "resources/secondary_structure.html",
        "resources/coordinate_section.html",
        "resources/miscellaneous_records.html",
        "resources/entry_revision_history.html"
    ]
    if(index < pages.length){
        fetch(pages[index])
            .then(response => {
                if(response.status && response.status < 400){
                    return response.text();
                } else {
                    throw new Error(response.statusText);
                }
            })
            .then(text => {
                document.getElementById("article").innerHTML += text;
                bs_content_fetch(index + 1);
            })
            .catch(err => { console.log(err); bs_content_fetch(index + 1); });
    } else if(index >= pages.length){
        // page loads complete
        onload_continued();
    }
}

function onload_continued(){
    // asynchronous fetches complete
    // modal styling
    bs_content_wrap();
    // wrap nested elements
    bs_wrap_toggle();
    // position page content relative to banner content
    let height = document.querySelector("div.fixed-top").offsetHeight;
    document.querySelector(".user-guide-container").style.top = `${height+TOPMARGIN}px`;
    // responsive resize events
    window.addEventListener("resize", () => {
        if(window.innerWidth != WINDOWWIDTH){
            WINDOWWIDTH = window.innerWidth;
            let height = document.querySelector("div.fixed-top").offsetHeight;
            document.querySelector(".user-guide-container").style.top = `${height+TOPMARGIN}px`;
        }
    });
    // sidebar link events (native links positioned wrong due to fixed top banner content)
    let anchors = document.querySelectorAll("#sidebar #navbar li");
    anchors.forEach((a) => {
        a.addEventListener("click", function(){
            window.event.preventDefault();
            window.event.stopImmediatePropagation();
            let href = a.getElementsByTagName("a")[0].getAttribute("name");
            if(href == null){
                return;
            }
            if(SIDEBAR){
                $("#article").scrollTop(0);
            }
            let element = $(`#article #${href}`);
            if(element == null){
                return;
            }
            let top = element.offset().top;
            let height = document.querySelector("div.fixed-top").offsetHeight;
            if(!SIDEBAR){
                $(document).scrollTop(top - height - TOPMARGIN);
            } else if(SIDEBAR && href != "introduction"){
                $("#article").scrollTop(top - height - TOPMARGIN);
            }
        }.bind(a));
    });
    // arrow toggling events
    let toggles = document.querySelectorAll("div[data-bs-toggle='collapse']");
    toggles.forEach((t) => {
          t.addEventListener("click", function () {
              let entity = this.getElementsByClassName("entity")[0];
              if(! entity.getAttribute("arrow")){
                  entity.setAttribute("arrow", "right");
              }
              let arrow = entity.getAttribute("arrow");
              if(arrow == "right") {
                  entity.innerHTML = "&blacktriangledown;";
                  entity.setAttribute("arrow", "down");
              } else {
                  entity.innerHTML = "&blacktriangleright;";
                  entity.setAttribute("arrow", "right");
              }
              if(entity.parentNode.className != "nested_arrow") {
                  if (PREVTOGGLE) {
                      PREVTOGGLE.innerHTML = "&blacktriangleright;";
                      PREVTOGGLE.setAttribute("arrow", "right");
                  }
                  PREVTOGGLE = entity;
              }
          }.bind(t));
    });
}

function bs_content_wrap() {
    if(!SIDEBAR){
        // insert 'top' links
        // nested example tables are not cards yet
        let cards = document.querySelectorAll("#article .card");
        cards.forEach((card) => {
            let footer = document.createElement("footer");
            let a = document.createElement("a");
            a.setAttribute("href", "#top");
            let txt = document.createTextNode("top");
            a.appendChild(txt);
            footer.appendChild(a);
            let body = card.querySelector(".card-body");
            body.appendChild(footer);
        });
    } else if(SIDEBAR){
        make_sidebar();
    }
}

function make_sidebar(){
    if(SIDEBAR && window.outerWidth < 800){
        SIDEBAR = false;
        window.location.href = window.location.href.replace(window.location.search, "?default");
    }
    window.addEventListener("resize", function(){
        console.log("width " + window.outerWidth);
        if(SIDEBAR && window.outerWidth < 800){
            SIDEBAR = false;
            window.location.href = window.location.href.replace(window.location.search, "?default");
        } else if(window.innerHeight != WINDOWHEIGHT) {
            window.location.reload();
        }
    });
    let userguide = document.getElementsByClassName("user-guide")[0];
    userguide.classList.add("row");
    let sidebar = document.getElementById("sidebar");
    sidebar.classList.add("col-sm-4");
    $('#sidebar #navbar ul li').css('padding', '3px');
    let article = document.getElementById("article");
    article.classList.add("col-sm-8");
    article.style.paddingLeft = "30px";
    let height = window.innerHeight;
    article.style.height = height + "px";
    article.style.overflow = "scroll";
    $('.card.parent').css('margin', '0px 0px 50px 0px');
}

// wrap nested accordions
function bs_wrap_toggle() {
    let examples = document.querySelectorAll('.example');
    for (let x = 0; x < examples.length; ++x) {

        // paragraph that says 'example'
        let example = examples[x];
        let html = example.innerHTML;

        let parent = example.parentNode;
        console.log('wrapping parent ' + x + ' ' + parent.parentNode.id + ' ' + parent.className);

        // table after paragraph
        let sib = example.nextElementSibling;

        let card = document.createElement('div');
        card.setAttribute('class', 'card nested');

        let header = document.createElement('div');
        header.setAttribute('class', 'card-header');
        header.setAttribute('data-bs-toggle', 'collapse');
        header.setAttribute('data-bs-target', `#example_${x}`);
        header.innerHTML = `
            <div class='nested_arrow'>
                <span class="entity">&blacktriangleright;</span>
                &nbsp;
                <span class="hoverable">${html}</span>
            </div>
        `;
        card.appendChild(header);

        let body = document.createElement('div');
        body.setAttribute('id', `example_${x}`);
        body.setAttribute('class', 'card-body collapse');

        // extract table into card
        body.appendChild(sib);
        card.appendChild(body);

        // replace paragraph
        parent.replaceChild(card, example);
    }
}
