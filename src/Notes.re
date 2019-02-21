module RD = ReactDOMRe;
module RR = ReasonReact;

let icon =
  <svg width="24" height="24" viewBox="0 0 24 24">
    <path
      d="M14 2H6c-1.1 0-1.99.9-1.99 2L4 20c0 1.1.89 2 1.99 2H18c1.1 0 2-.9 2-2V8l-6-6zm2 14h-3v3h-2v-3H8v-2h3v-3h2v3h3v2zm-3-7V3.5L18.5 9H13z"
    />
  </svg>;

type state = {
  notes: list(string),
  newVal: string,
};

type actions =
  | AddNote
  | UpdateText(string);

let addTestId = (testId, re) =>
  RR.cloneElement(re, ~props={"data-testid": testId}, [||]);

let component = ReasonReact.reducerComponent("Notes");

let make = (~initialNotes, _children) => {
  ...component,
  initialState: () => {notes: initialNotes, newVal: ""},
  reducer: (action, state) =>
    switch (action) {
    | AddNote =>
      ReasonReact.Update({
        notes: List.append(state.notes, [state.newVal]),
        newVal: "",
      })
    | UpdateText(txt) => RR.Update({...state, newVal: txt})
    },
  render: self =>
    <div style={RD.Style.make(~display="flex", ~flexDirection="column", ())}>
      <div
        style={RD.Style.make(
          ~display="flex",
          ~alignItems="space-between",
          (),
        )}>
        {<input
           type_="text"
           placeholder="Add new note here"
           value={self.state.newVal}
           onChange={e =>
             self.send(UpdateText(ReactEvent.Form.target(e)##value))
           }
         />
         |> addTestId("input-annotation")}
        {<div onClick={_ => self.send(AddNote)}> icon </div>
         |> addTestId("add-note-button")}
      </div>
      {self.state.notes
       |> List.mapi((idx, v) =>
            <span key={string_of_int(idx)}> {RR.string(v)} </span>
            |> addTestId("note-" ++ string_of_int(idx))
          )
       |> Array.of_list
       |> RR.array}
    </div>,
};