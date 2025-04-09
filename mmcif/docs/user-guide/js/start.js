// for offline or alternate servers
let FETCH = false;

document.addEventListener("DOMContentLoaded", function(){
        // populate user guide container
        if(FETCH) {
            document.getElementById("article").innerHTML = "";
            bs_content_fetch(0);
        } else {
            onload_continued();
        }
    }
);

function onload_continued() {
    // asynchronous fetches complete
    // insert back-to-top links (could make optional on wide screen)
    back_to_tops();
    // wrap nested elements (could expand to notes sections and subsections)
    bs_wrap_toggle();
    // sidebar link events
    let anchors = document.querySelectorAll("#sidebar #navbar li");
    anchors.forEach((a) => {
        a.addEventListener("click", function () {
            // prevent default or stop propagation would prevent default anchor link behavior (hashtag appended to url)
            if (a.nodeName.toLowerCase() == "li"){
                a.getElementsByTagName("a")[0].click();
            }
        }.bind(a));
    });
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
        // make accordion
        let details = document.createElement('details');
        details.setAttribute('class', 'card nested');
        parent.insertBefore(details, example);
        let summary = document.createElement('summary');
        summary.setAttribute('class', 'card-header hoverable');
        span = document.createElement('span');
        span.setAttribute('class', 'hoverable');
        span.innerHTML = html;
        summary.appendChild(span);
        details.appendChild(summary);
        let body = document.createElement('div');
        body.setAttribute('class', 'card-body');
        body.appendChild(sib);
        details.appendChild(body);
        parent.removeChild(example);
    }
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
