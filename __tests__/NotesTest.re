open Jest;
open Expect;

open Belt.Result;

module DT = DomTestingLibrary;
module RT = ReactTestingLibrary;

let errorHandler = err => Js.log(err);

[@bs.get] external innerText: Dom.element => string = "";
[@bs.get] external innerHTML: Dom.element => string = "";

/* Workaround for broken binding in dom-testing-libray.
   should be fixed upstream soon */
[@bs.module "dom-testing-library"] [@bs.scope "fireEvent"]
external change: (Dom.element, Js.t({..})) => unit = "";

let changeValue = (value, domElement) =>
  change(domElement, {
                       "target": {
                         "value": value,
                       },
                     });

describe("Note Functionality Test", () => {
  test("Notes component renders successfully", () =>
    RT.render(<Notes initialNotes=["First note"] />)
    |> RT.container
    |> expect
    |> toMatchSnapshot
  );

  testAsync(
    "When user adds a note, the new note contains the user's input", finish => {
    let tree = RT.render(<Notes initialNotes=["First note"] />);

    let container = tree |> RT.container;

    let waitForNote = () => tree |> RT.getByTestId("note-1");

    let waitForInputChange = () => tree |> RT.getByValue("New Note");

    let options =
      DT.WaitForElement.makeOptions(~container, ~timeout=2000, ());

    tree
    |> RT.getByPlaceholderText("Add new note here")
    |> changeValue("New Note");

    DT.waitForElement(~options, ~callback=waitForInputChange, ())
    ->FutureJs.fromPromise(errorHandler)
    ->Future.flatMapOk(_ => {
        tree |> RT.getByTestId("add-note-button") |> DT.FireEvent.click;

        DT.waitForElement(~callback=waitForNote, ~options, ())
        ->FutureJs.fromPromise(errorHandler);
      })
    ->Future.get(
        fun
        | Ok(node) =>
          node |> innerHTML |> expect |> toEqual("New Note") |> finish
        | Error(err) => {
            Js.log(err);
            fail("Note not found") |> finish;
          },
      );
  });
});