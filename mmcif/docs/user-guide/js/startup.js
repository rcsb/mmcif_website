// min responsive width from css - if change here must also change there
let MIN_SIDEBAR_LAYOUT_WIDTH = 800;
// sidebar toggler
let SIDEHANDLE = true;
let SIDEHANDLE_LEFT = `
<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-chevron-double-left" viewBox="0 0 16 16">
  <path fill-rule="evenodd" d="M8.354 1.646a.5.5 0 0 1 0 .708L2.707 8l5.647 5.646a.5.5 0 0 1-.708.708l-6-6a.5.5 0 0 1 0-.708l6-6a.5.5 0 0 1 .708 0"/>
  <path fill-rule="evenodd" d="M12.354 1.646a.5.5 0 0 1 0 .708L6.707 8l5.647 5.646a.5.5 0 0 1-.708.708l-6-6a.5.5 0 0 1 0-.708l6-6a.5.5 0 0 1 .708 0"/>
</svg>
`;
let SIDEHANDLE_RIGHT = `
<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-chevron-double-right" viewBox="0 0 16 16">
  <path fill-rule="evenodd" d="M3.646 1.646a.5.5 0 0 1 .708 0l6 6a.5.5 0 0 1 0 .708l-6 6a.5.5 0 0 1-.708-.708L9.293 8 3.646 2.354a.5.5 0 0 1 0-.708"/>
  <path fill-rule="evenodd" d="M7.646 1.646a.5.5 0 0 1 .708 0l6 6a.5.5 0 0 1 0 .708l-6 6a.5.5 0 0 1-.708-.708L13.293 8 7.646 2.354a.5.5 0 0 1 0-.708"/>
</svg>
`;
// maintain article scroll position when toggle sidebar
let TRACKING = true;
if(!SIDEHANDLE){
    TRACKING = false;
}
// for offline or alternate servers
let FETCH = false;
// styling relative to top navbar
let TOPMARGIN = 20;
// utilities
let PREVTOGGLE = null;
let TARGETS = null;

document.addEventListener("DOMContentLoaded", function(){
        // populate user guide container
        if(FETCH) {
            document.getElementById("article").innerHTML = "";
            bs_content_fetch(0);
        } else {
            onload_continued();
        }
        window.setTimeout(hash, 0);
    }
);

function sidebar_layout(){
    return window.innerWidth >= MIN_SIDEBAR_LAYOUT_WIDTH;
}

function detect_browser_name(){
    let agent = window.navigator.userAgent;
    if(agent.indexOf("Chrome") >= 0){
        return "Chrome";
    } else if(agent.indexOf("Edg") >= 0){
        return "Edge";
    } else if(agent.indexOf("Firefox") >= 0){
        return "Firefox";
    } else {
        return "Safari";
    }
}

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

function onload_continued() {
    // asynchronous fetches complete
    // insert back-to-top links (could make optional on wide screen)
    back_to_tops();
    // wrap nested elements (could expand to notes sections and subsections)
    bs_wrap_toggle();
    // position page content relative to banner content
    let topnav_height = document.querySelector("div.fixed-top").offsetHeight;
    let sidebar = document.getElementById("sidebar");
    let article = document.getElementById("article");
    if (sidebar_layout()) {
        sidebar.style.top = `${topnav_height + TOPMARGIN}px`;
        article.style.top = `${topnav_height + TOPMARGIN}px`;
        sidebar.style.paddingTop = "0px";
        sidebar.style.height = `${window.innerHeight - topnav_height - TOPMARGIN}px`;
        sidehandle();
    } else {
        sidebar.style.paddingTop = `${topnav_height + TOPMARGIN}px`;
        sidebar.style.height = '100%';
    }
    // responsive resize events
    window.addEventListener("resize", () => {
        let topnav_height = document.querySelector("div.fixed-top").offsetHeight;
        let sidebar = document.getElementById("sidebar");
        let article = document.getElementById("article");
        if (sidebar_layout()) {
            sidebar.style.top = `${topnav_height + TOPMARGIN}px`;
            article.style.top = `${topnav_height + TOPMARGIN}px`;
            sidebar.style.paddingTop = "0px";
            sidebar.style.height = `${window.innerHeight - topnav_height - TOPMARGIN}px`;
            sidehandle();
        } else {
            sidebar.style.paddingTop = `${topnav_height + TOPMARGIN}px`;
            sidebar.style.height = '100%';
            // prevent sidehandle toggling and related effects
            let sidehandle = document.getElementById("sidehandle");
            sidehandle.style.display = "none";
            sidebar.classList.remove("fullscreen");
            article.classList.remove("fullscreen");
            sidebar.classList.remove("narrow");
            article.classList.remove("narrow");
            // reset sidehandle for return to wide screen
            sidehandle.innerHTML = SIDEHANDLE_LEFT;
            sidehandle.classList.remove("right");
            sidehandle.classList.add("left");
        }
    });
    // sidebar link events (native links positioned wrong due to fixed top banner content)
    let anchors = document.querySelectorAll("#sidebar #navbar li");
    anchors.forEach((a) => {
        a.addEventListener("click", function () {
            window.event.preventDefault();
            window.event.stopImmediatePropagation();
            let href = a.getElementsByTagName("a")[0].getAttribute("name");
            if (href == null) {
                return;
            }
            let element = $(`#article #${href}`);
            if (element == null) {
                return;
            }
            let top = element.offset().top;
            let topnav_height = document.querySelector("div.fixed-top").offsetHeight;
            $(document).scrollTop(top - topnav_height - TOPMARGIN);
        }.bind(a));
    });
    // arrow toggling events
    let toggles = document.querySelectorAll("div[data-bs-toggle='collapse']");
    toggles.forEach((t) => {
        t.addEventListener("click", function () {
            let entity = this.getElementsByClassName("entity")[0];
            if (!entity.getAttribute("arrow")) {
                entity.setAttribute("arrow", "right");
            }
            let arrow = entity.getAttribute("arrow");
            if (arrow == "right") {
                entity.innerHTML = "&blacktriangledown;";
                entity.setAttribute("arrow", "down");
            } else {
                entity.innerHTML = "&blacktriangleright;";
                entity.setAttribute("arrow", "right");
            }
            if (!entity.parentNode.classList.contains("nested_arrow")) {
                if (PREVTOGGLE) {
                    PREVTOGGLE.innerHTML = "&blacktriangleright;";
                    PREVTOGGLE.setAttribute("arrow", "right");
                }
                PREVTOGGLE = entity;
            }
        }.bind(t));
    });
    // sidebar visibility toggler
    if(SIDEHANDLE) {
        toggle_hide_sidebar();
    }
}

function hash(){
    if (window.location.hash) {
        let hash = window.location.hash;
        console.log(`hash ${hash}`);
        let element = document.querySelector(hash);
        if (element) {
            window.scrollTo(0,0);
            let top = element.getBoundingClientRect().top + window.pageYOffset;
            let topnav_height = document.querySelector("div.fixed-top").offsetHeight;
            let targ = top - topnav_height - TOPMARGIN;
            console.log(`${top} - ${topnav_height} - ${TOPMARGIN} = ${targ}`);
            window.setTimeout(()=> {
                window.scrollTo({
                    top: targ,
                    behavior: 'smooth'
                });
            }, 100);
        }
    }
}

function toggle_hide_sidebar(){
    let sidehandle = document.getElementById("sidehandle");
    sidehandle.addEventListener("click", function () {
        if(TARGETS == null){
            TARGETS = document.querySelectorAll("#article *");
        }
        let min = 99999999;
        let target = null;
        for(let x = 0;x < TARGETS.length;++x){
            let q = TARGETS[x];
            let { top, left, bottom, right } = q.getBoundingClientRect();
            top += 60;
            if(top > 0 && top < min){
                min = top;
                target = q;
            }
        }
        if (this.classList.contains("left")) {
            this.innerHTML = SIDEHANDLE_RIGHT;
            sidebar.classList.remove("narrow");
            article.classList.remove("narrow");
            sidebar.classList.add("fullscreen");
            article.classList.add("fullscreen");
            this.classList.remove("left");
            this.classList.add("right");
        } else {
            this.innerHTML = SIDEHANDLE_LEFT;
            sidebar.classList.remove("fullscreen");
            article.classList.remove("fullscreen");
            sidebar.classList.add("narrow");
            article.classList.add("narrow");
            this.classList.remove("right");
            this.classList.add("left");
        }
        if(window.scrollY === 0){
            return;
        }
        if(!TRACKING){
            return;
        }
        // maintain article scroll position
        if(this.classList.contains("right")){
            window.setTimeout(function(){
                target.scrollIntoView({behavior:'instant'});
            }.bind(target), 200);
        } else {
            window.setTimeout(function(){
                target.scrollIntoView({behavior:'instant'});
            }.bind(target), 200);
        }
    }.bind(sidehandle));
}

function sidehandle() {
    // centering of sidehandle
    if (SIDEHANDLE) {
        let sidehandle = document.getElementById("sidehandle");
        sidehandle.style.display = "block";
        let topnav_height = document.querySelector("div.fixed-top").offsetHeight;
        let navbar = document.querySelector("#sidebar #navbar");
        const offsetTop = Number(navbar.offsetTop);
        const navbar_height = Number(navbar.clientHeight);
        const sidehandle_height = Number(sidehandle.clientHeight);
        const location = topnav_height + TOPMARGIN + offsetTop + (navbar_height / 2) - (sidehandle_height / 2);
        sidehandle.style.top = `${location}px`;
    }
}

function back_to_tops() {
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
}

// wrap nested accordions
function bs_wrap_toggle() {
    let examples = document.querySelectorAll('p.example');
    for (let x = 0; x < examples.length; ++x) {

        // paragraph that says 'example'
        let example = examples[x];
        let html = example.innerHTML;

        let parent = example.parentNode;

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
